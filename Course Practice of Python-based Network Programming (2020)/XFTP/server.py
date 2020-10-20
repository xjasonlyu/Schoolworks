#!/usr/bin/env python3
import json
import logging
import os
import socket
import struct
import threading
import random

from const import *

random.seed(os.urandom(8))

logging.basicConfig(level=logging.DEBUG)


def go(f):
    t = threading.Thread(target=f)
    t.start()


class Server:
    def __init__(self, host='127.0.0.1', port=2333, base='./'):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.s.bind((host, port))
        self.host = host
        self.port = port
        self.base = os.path.abspath(base)

    @staticmethod
    def random_port():
        return random.randint(30000, 59999)

    @staticmethod
    def parse_packet(data):
        pass

    def translate_path(self, path: str) -> str:
        if path.startswith('/'):
            path = path.lstrip('/')
        return os.path.join(self.base, path)

    def reply(self, rep: int, cmd: int, _id: bytes, data: bytes, addr: tuple):
        payload = bytearray([MAGIC_NUMBER, rep, cmd, *_id, *data])
        self.s.sendto(payload, addr)

    def handle_list(self, _id, _dir, addr, data=b''):
        # logging.debug(f'{_id}, {file_name}, {addr}, {packet}')
        reply = REPLY_OK
        _files = {'dirs': [], 'files': []}
        path = self.translate_path(_dir)
        if not os.path.exists(path):
            reply = REPLY_NOT_FOUND
        elif os.path.isfile(path):
            reply = REPLY_INVALID
        else:
            try:
                files = os.listdir(path)
                for f in files:
                    p = os.path.join(path, f)
                    if os.path.isdir(p):
                        _files['dirs'].append(f)
                    else:
                        _files['files'].append(f)
            except Exception as e:
                logging.error(f'handle list error: {e}')
                reply = REPLY_INTERNAL_ERROR
            data = json.dumps(_files).encode('utf-8')

        self.reply(reply, LIST, _id, data, addr)
        return

    def handle_upload(self, _id, file_name, addr, data=b''):
        path = self.translate_path(file_name)
        if os.path.exists(path):
            self.reply(REPLY_FILE_EXISTS, UPLOAD, _id, data, addr)
            return

        try:
            fp = open(path, 'wb')
        except Exception as e:
            logging.error(f'handle upload error: {e}')
            self.reply(REPLY_INTERNAL_ERROR, UPLOAD, _id, data, addr)
            return

        port = self.random_port()
        ts = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            ts.bind((self.host, port))  # bind to random port
        except Exception as e:
            logging.error(f'handle upload bind error: {e}')
            self.reply(REPLY_INTERNAL_ERROR, UPLOAD, _id, data, addr)
            fp.close()
            # os.remove(path)
            return

        logging.debug(f'file transport udp for upload listen at port: {port}')
        data = struct.pack('<H', port)
        self.reply(REPLY_OK, UPLOAD, _id, data, addr)

        while True:
            ts.settimeout(TIMEOUT)
            try:
                packet, addr = ts.recvfrom(BUF_SIZE)
            except socket.timeout:
                logging.debug(f'listen timeout: {(self.host, port)}')
                break
            if not packet:
                break
            fp.write(packet)
            ts.sendto(bytearray([MAGIC_NUMBER, REPLY_ACK, UPLOAD, *_id]), addr)

        # release
        fp.close()
        ts.close()
        return

    def handle_download(self, _id, file_name, addr, data):
        path = self.translate_path(file_name)
        if not os.path.exists(path):
            self.reply(REPLY_NOT_FOUND, DOWNLOAD, _id, b'', addr)
            return

        port = self.random_port()
        ts = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            ts.bind((self.host, port))  # bind to random port
        except Exception as e:
            logging.error(f'handle download bind error: {e}')
            self.reply(REPLY_INTERNAL_ERROR, DOWNLOAD, _id, b'', addr)

        logging.debug(f'file transport udp for download listen at port: {port}')
        data = struct.pack('<H', port)
        self.reply(REPLY_OK, DOWNLOAD, _id, data, addr)

        with open(path, 'rb') as f:
            while True:
                d = f.read(BUF_SIZE)
                ts.sendto(d, addr)
                if not d:
                    break
                try:
                    ts.settimeout(TIMEOUT)
                    d, _addr = ts.recvfrom(BUF_SIZE)
                except socket.timeout:
                    logging.error('wait ACK timeout, transport failed.')
                    break
                if _addr != addr:
                    logging.warning(f'recv from invalid address: {_addr} excepted: {addr}')
                    break
                if not d or len(d) < 2 or d[1] != REPLY_ACK:
                    logging.warning(f'not excepted ACK: {d}')
                    break
                logging.debug(f'GET ACK from {_addr}')

        ts.close()
        return

    @staticmethod
    def handle_delete(_id, file_name, addr, data):
        pass

    def handle(self, addr, packet):
        if not packet or packet[0] != MAGIC_NUMBER:
            logging.warning(f'malformed packet {packet} from {addr}')
            return
        elif len(packet) < 1 + 1 + 1 + 8 + 1:
            logging.warning(f'short packet {packet} from {addr}')
            return

        req = packet[1]
        if req != REQUEST:
            logging.debug(f"packet from {addr} isn't a request")
            return

        cmd = packet[2]
        _id = packet[3:11]
        if len(packet[10:]) <= 0:
            logging.debug(f'packet from {addr} file name length is zero or too short')
            return

        file_name = packet[11:].decode('utf-8')
        if cmd == LIST:
            go(lambda: self.handle_list(_id, file_name, addr, b''))
        elif cmd == UPLOAD:
            go(lambda: self.handle_upload(_id, file_name, addr, b''))
        elif cmd == DOWNLOAD:
            go(lambda: self.handle_download(_id, file_name, addr, b''))
        elif cmd == DELETE:
            pass
        else:
            logging.warning(f'unknown CMD {cmd} from {addr}')
        return

    def run_forever(self):
        while True:
            packet, addr = self.s.recvfrom(BUF_SIZE)
            logging.debug(f'server recv from: {addr}')
            self.handle(addr, packet)


def main():
    server = Server(base='/Users/jason/Downloads/')
    try:
        server.run_forever()
    except KeyboardInterrupt:
        return


if __name__ == '__main__':
    main()
