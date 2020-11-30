#!/usr/bin/env python3

import os
import sys
import select
import socket
import struct
import logging
import threading

from protocol import *


logging.basicConfig(level=logging.DEBUG)


def get_file_size(filename: str) -> bytes:
    n = os.path.getsize(filename)
    return struct.pack('>Q', n)


class BaseServer:

    def __init__(self, host, port, path):
        self.host = host
        self.port = port
        self.path = path

    def _do_listen(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((self.host, self.port))
        s.listen(1024)
        self.s = s

    def _do_reply(self, conn):
        buf = conn.recv(1)
        v = buf[0]
        if v != VERSION:
            logging.warning(f'version mismatch: {v}')
            return

        buf = conn.recv(1)
        n = buf[0]
        buf = conn.recv(n)
        filename = buf.decode()
        if len(filename) != n:
            logging.warning(f'filename too short, filename: {filename}')
            conn.sendall(bytearray([VERSION, REP_INVALID_REQUEST]))
            return

        filename = os.path.join(self.path, filename)

        if not os.path.exists(filename):
            logging.warning(f'file does not exists: {filename}')
            conn.sendall(bytearray([VERSION, REP_NOT_FOUND]))
            return

        _ = REP_INTERNAL_ERROR

        conn.sendall(bytearray([VERSION, REP_SUCCESS]))
        logging.info(f'download request: {filename}')

        file_size = get_file_size(filename)
        conn.sendall(file_size)

        with open(filename, 'rb') as f:
            while True:
                data = f.read(4096)  # default buffsize
                if not data:
                    break
                conn.sendall(data)

    def serve_forever(self):
        self._do_listen()

        while True:
            conn, addr = self.s.accept()
            logging.info(f'client from: {addr}')
            with conn:
                self._do_reply(conn)


class SingleThreadServer(BaseServer):
    pass


class MultiThreadServer(BaseServer):

    @staticmethod
    def go(f):
        t = threading.Thread(target=f)
        t.start()

    def handle(self, conn):
        with conn:
            self._do_reply(conn)

    def serve_forever(self):
        self._do_listen()

        while True:
            conn, addr = self.s.accept()
            logging.info(f'client from: {addr}')
            # go
            self.go(f=lambda: self.handle(conn))


class AsyncSelectServer(BaseServer):

    # inner handler
    class handler:

        STAGE_INIT = 0x00
        STAGE_FILENAME = 0x01
        STAGE_RELAY = 0x02

        def __init__(self, conn, inputs, outputs, handlers, path):
            self.conn = conn
            self.path = path
            self.inputs = inputs
            self.outputs = outputs
            self.handlers = handlers
            self.stage = self.STAGE_INIT

            self.f = None
            self.N = 0  # file bytes to write
            self.filename_n = 0

        def handle(self):
            if self.stage == self.STAGE_INIT:
                buf = self.conn.recv(2)
                assert len(buf) == 2

                v, n = buf
                if v != VERSION:
                    logging.warning(f'version mismatch: {v}')
                    self.destory()
                    return

                self.filename_n = n
                self.stage = self.STAGE_FILENAME

            elif self.stage == self.STAGE_FILENAME:
                buf = self.conn.recv(self.filename_n)
                if len(buf) != self.filename_n:
                    logging.warning('filename too short')
                    self.conn.send(bytearray([VERSION, REP_INVALID_REQUEST]))
                    self.destory()
                    return

                filename = os.path.join(self.path, buf.decode())
                if not os.path.exists(filename):
                    logging.warning(f'file does not exists: {filename}')
                    self.conn.send(bytearray([VERSION, REP_NOT_FOUND]))
                    self.destory()
                    return

                # write data to tcp buffer, no blocking
                self.conn.sendall(bytearray([VERSION, REP_SUCCESS]))
                logging.info(f'download request: {filename}')

                file_size = get_file_size(filename)
                self.conn.sendall(file_size)

                self.f = open(filename, 'rb')
                self.stage = self.STAGE_RELAY
                # add to outputs
                self.outputs.add(self.conn)

            elif self.stage == self.STAGE_RELAY:
                buffsize = 128  # default size

                self.f.seek(self.N)  # goto last offset
                data = self.f.read(buffsize)
                if not data:  # read complete
                    self.destory()
                    return

                self.N += self.conn.send(data)

        def destory(self):
            if self.conn:  # release connection
                logging.debug(f'destory connection: {self.conn}')
                self.inputs.remove(self.conn)
                if self.conn in self.outputs:
                    self.outputs.remove(self.conn)
                del self.handlers[self.conn]
                self.conn.close()
                self.conn = None

            if self.f:  # close file
                self.f.close()

    def handle(self, conn):
        h = self.handlers.get(conn, None)
        if h is None:
            return

        try:
            h.handle()
        except Exception as e:
            logging.warning(f'handle error:{e}')
            h.destory()

    def serve_forever(self):
        self._do_listen()

        # no blocking
        self.s.setblocking(0)

        # inputs
        inputs = set()
        outputs = set()
        inputs.add(self.s)

        self.handlers = {}

        while True:
            r, w, _ = select.select(inputs, outputs, [], 5)

            for s in r:
                if s is self.s:  # server instance
                    conn, addr = s.accept()
                    conn.setblocking(0)
                    # must send immediately
                    conn.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
                    logging.info(f'client from: {addr}')
                    inputs.add(conn)  # add to inputs
                    self.handlers[conn] = self.handler(
                        conn, inputs, outputs, self.handlers, self.path)
                else:  # client connections
                    self.handle(s)

            for s in w:
                self.handle(s)


def main():
    if len(sys.argv) < 3:
        print(f'Usage: {sys.argv[0]} mode host port <serve_dir>')
        return

    mode = sys.argv[1]
    host = sys.argv[2]
    port = int(sys.argv[3])
    try:
        path = sys.argv[4]
    except:
        path = '.'

    s = None
    if mode == '1':
        s = SingleThreadServer(host, port, path)
        logging.info('using single-thread mode')
    elif mode == '2':
        s = MultiThreadServer(host, port, path)
        logging.info('using multi-thread mode')
    elif mode == '3':
        s = AsyncSelectServer(host, port, path)
        logging.info('using async(select) mode')
    else:
        raise Exception(f'unknown mode: {mode}')

    try:
        s.serve_forever()
    except KeyboardInterrupt:
        pass


if __name__ == '__main__':
    main()
