#!/usr/bin/env python3
import json
import logging
import os
import socket
import struct
import sys

from const import *

logging.basicConfig(level=logging.DEBUG)


class Client:
    def __init__(self, host='127.0.0.1', port=2333):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.host = host
        self.port = port
        self._id = os.urandom(8)

    def ack(self, addr):
        payload = bytearray([MAGIC_NUMBER, REPLY_ACK, NOTHING, *self._id])
        self.s.sendto(payload, addr)

    def request(self, cmd, filename: str):
        payload = bytearray([MAGIC_NUMBER, REQUEST, cmd, *self._id, *filename.encode('utf-8')])
        self.s.sendto(payload, (self.host, self.port))

        rep = REPLY_INVALID
        cmd = NOTHING
        data = b''
        try:
            self.s.settimeout(5)
            packet, addr = self.s.recvfrom(BUF_SIZE)
        except socket.timeout:
            logging.info('request TIMEOUT, please retry later')
            return rep, cmd, data
        logging.debug(f'packet: {packet}')
        if addr != (self.host, self.port):
            logging.warning('unmatched address!')
            return rep, cmd, data
        if not packet or packet[0] != MAGIC_NUMBER:
            logging.warning(f'malformed packet {packet} from {addr}')
            return rep, cmd, data
        if len(packet) < 1 + 1 + 1 + 8:
            logging.warning(f'short packet {packet} from {addr}')
            return rep, cmd, data

        rep = packet[1]
        cmd = packet[2]
        _id = packet[3:11]
        if _id != self._id:
            logging.warning('unmatched id token!')

        if len(packet) >= 12:
            data = packet[11:]
        return rep, cmd, data

    def list(self, _dir: str):
        rep, cmd, data = self.request(LIST, _dir)
        if rep != REPLY_OK or cmd != LIST:
            logging.error(f"REPLY:{rep}={code2text(rep)} CMD:{cmd}")
            return
        files = json.loads(data)

        for d in files.get('dirs', []):
            print(f'dir: {d}')
        for f in files.get('files', []):
            print(f'file: {f}')

    def upload(self, up_file: str, local_file: str):
        rep, cmd, data = self.request(UPLOAD, up_file)
        if rep != REPLY_OK or cmd != UPLOAD:
            logging.error(f"REPLY:{rep}={code2text(rep)} CMD:{cmd}")
            return
        if len(data) != 2:
            logging.error(f"data is not right: {data}")
            return
        port = struct.unpack('<H', data)[0]

        with open(local_file, 'rb') as f:
            while True:
                d = f.read(BUF_SIZE)
                self.s.sendto(d, (self.host, port))
                if not d:
                    break
                self.s.settimeout(TIMEOUT)
                try:
                    d, addr = self.s.recvfrom(BUF_SIZE)
                except socket.timeout:
                    logging.error('wait ACK timeout, transport failed, please retry later.')
                    return
                if addr != (self.host, port):
                    logging.warning(f'recv from invalid address: {addr} excepted: {(self.host, port)}')
                    break
                if not d or len(d) < 2 or d[1] != REPLY_ACK:
                    logging.warning(f'not excepted data: {d}')
                    break
                # logging.debug('GET ACK!')
                print('.', end='')
        print(flush=True)
        logging.info('done!')

    def download(self, down_file: str, local_file: str):
        if os.path.exists(local_file):
            logging.warning(f'local file {local_file} exists')
            return

        rep, cmd, data = self.request(DOWNLOAD, down_file)
        if rep != REPLY_OK or cmd != DOWNLOAD:
            logging.error(f"REPLY:{rep}={code2text(rep)} CMD:{cmd}")
            return
        if len(data) != 2:
            logging.error(f"data is not right: {data}")
            return
        port = struct.unpack('<H', data)[0]

        with open(local_file, 'wb') as f:
            while True:
                self.s.settimeout(TIMEOUT)
                try:
                    d, addr = self.s.recvfrom(BUF_SIZE)
                except socket.timeout:
                    logging.error('download timeout, transport failed, please retry later.')
                    return
                if addr[1] != port:
                    logging.warning(f'recv from invalid address: {addr} excepted: {(self.host, port)}')
                    break
                if not d:
                    break
                self.ack(addr)
                f.write(d)
                print('.', end='', flush=True)
        print(flush=True)
        logging.info('done!')


def main():
    args = sys.argv
    if len(args) < 4:
        print(f'Usage: {args[0]} host port cmd <args>...')
        return

    host, port, cmd = args[1:4]
    port = int(port)
    c = Client(host, port)

    if cmd.lower() == 'upload':
        dst, src = args[4], args[5]
        c.upload(dst, src)
    elif cmd.lower() == 'list':
        _dir = args[4]
        c.list(_dir)
    elif cmd.lower() == 'download':
        dst, src = args[4], args[5]
        c.download(dst, src)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass
