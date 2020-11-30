#!/usr/bin/env python3

import os
import sys
import socket
import struct

from protocol import *


class ClientError(Exception):
    pass


class Client:

    def __init__(self, host, port):
        self.host = host
        self.port = port

    @staticmethod
    def _connect(host, port):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((host, port))
        return s

    def _do_request(self, filename):
        req = bytearray([VERSION, len(filename)])
        req.extend(filename.encode())

        self.s = self._connect(self.host, self.port)
        self.s.sendall(req)

    def _do_receive(self, filename):
        buf = self.s.recv(1)
        v = buf[0]
        if v != VERSION:
            raise ClientError('version mismatch!')

        buf = self.s.recv(1)
        rep = buf[0]  # bytes to int
        if rep == REP_SUCCESS:
            pass
        elif rep == REP_NOT_FOUND:
            raise ClientError('file not found')
        elif rep == REP_INTERNAL_ERROR:
            raise ClientError('server internal error')
        elif rep == REP_INVALID_REQUEST:
            raise ClientError('client invalid request')
        else:
            raise ClientError(f'unknown REP code: {rep}')

        buf = self.s.recv(8)
        assert len(buf) == 8

        file_size = struct.unpack('>Q', buf)[0]

        n = 0
        with open(filename, 'wb') as f:
            while True:
                data = self.s.recv(4096)
                if not data:
                    break
                f.write(data)

                n += len(data)
                self.progress_bar(n, file_size)
                # print(f'read data: {data}')
        print('\ndone!')

        if n != file_size:
            raise ClientError('uncompleted file!')

        # release resources
        self.s.close()
        self.s = None

    @staticmethod
    def progress_bar(n, all, width=40):
        bar = '='*(round(n/all*width)-1)+'>' if n != all else '='*width
        print(
            f'\rXDOWN: [{bar.ljust(width)}] {(n/all)*100:.0f}%\t{n:,} bytes received', end='', flush=True)

    def download(self, remote_file, local_file):
        if len(remote_file) > 0xff:
            raise ClientError('filename too long')
        if os.path.exists(local_file):
            raise ClientError('file already exists')

        self._do_request(remote_file)
        self._do_receive(local_file)


def main():
    if len(sys.argv) < 4:
        print(f'Usage: {sys.argv[0]} host port remote_file <local_file>')
        return

    host = sys.argv[1]
    port = int(sys.argv[2])
    r_file = sys.argv[3]
    try:
        l_file = sys.argv[4]
    except:
        l_file = r_file  # use as default

    c = Client(host, port)

    try:
        c.download(r_file, l_file)
    except KeyboardInterrupt:
        pass
    except ClientError as e:
        print(f'download error: {e}', file=sys.stderr)


if __name__ == '__main__':
    main()
