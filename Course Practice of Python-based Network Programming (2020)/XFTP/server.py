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
    def __init__(self, host='127.0.0.1', port=2333):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.s.bind((host, port))
        self.host = host
        self.port = port

    @staticmethod
    def random_port():
        return random.randint(30000, 59999)

    @staticmethod
    def parse_packet(data):
        pass

    def reply(self, rep: int, cmd: int, _id: bytes, data: bytes, addr: tuple):
        payload = bytearray([MAGIC_NUMBER, rep, cmd, *_id, len(data), *data])
        self.s.sendto(payload, addr)

    def handle_list(self, _id, file_name, addr, data=b''):
        # logging.debug(f'{_id}, {file_name}, {addr}, {packet}')
        reply = REPLY_OK
        _files = {'dirs': [], 'files': []}
        if not os.path.exists(file_name):
            reply = REPLY_NOT_FOUND
        elif os.path.isfile(file_name):
            reply = REPLY_INVALID
        else:
            try:
                files = os.listdir(file_name)
                for f in files:
                    p = os.path.join(file_name, f)
                    if os.path.isdir(p):
                        _files['dirs'].append(bytes(f).decode('utf-8'))
                    else:
                        _files['files'].append(bytes(f).decode('utf-8'))
            except Exception as e:
                logging.error(f'handle list error: {e}')
                reply = REPLY_INTERNAL_ERROR
            data = json.dumps(_files).encode('utf-8')

        self.reply(reply, LIST, _id, data, addr)
        return

    def handle_upload(self, _id, file_name, addr, data=b''):
        if os.path.exists(file_name):
            self.reply(REPLY_FILE_EXISTS, UPLOAD, _id, data, addr)
            return

        try:
            fp = open(file_name, 'wb')
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
            return

        logging.debug(f'file transport udp listen at port: {port}')
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

        fp.close()
        return

    @staticmethod
    def handle_download(_id, file_name, addr, packet):
        pass

    @staticmethod
    def handle_delete(_id, file_name, addr, packet):
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
        file_name_len = packet[11]
        if not file_name_len or len(packet[12:]) < file_name_len:
            logging.debug(f'packet from {addr} file name length is zero or too short')
            return

        file_name = packet[12:12 + file_name_len]
        data = packet[12 + file_name_len:]
        if cmd == LIST:
            go(lambda: self.handle_list(_id, file_name, addr, data))
        elif cmd == UPLOAD:
            go(lambda: self.handle_upload(_id, file_name, addr, data))
        elif cmd == DOWNLOAD:
            pass
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
    server = Server()
    try:
        server.run_forever()
    except KeyboardInterrupt:
        return


if __name__ == '__main__':
    main()
