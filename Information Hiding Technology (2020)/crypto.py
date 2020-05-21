#!/usr/bin/env python3

from typing import Callable

import numpy as np
from Crypto.Cipher import AES


def pad(x): return x+(8-x % 8)


def tobyte(t):
    assert len(t) == 8
    x = 0
    for b in t:
        assert b in (0, 1)
        x = (x << 1) + int(b)
    return x


def toarray(b):
    assert 0x00 <= b <= 0xff
    t = np.zeros(8, dtype=np.bool)
    for i in range(8):
        t[7-i] = (b >> i) % 2
    return t


# encrypt image bit by bit
# fake cbc mode encryption
# weak key: [iv][key] 1 byte iv + 1 byte key
def ixor(img: np.ndarray, key: bytes, encrypt: bool) -> np.ndarray:
    assert len(img.shape) == 2
    assert len(key) == 2 and type(key) == bytes

    z = np.zeros(pad(img.shape[0]*img.shape[1]))

    for i, b in enumerate(np.nditer(img)):
        z[i] = b

    iv = key[0]
    k = key[1]

    for i in range(len(z)//8):
        if encrypt:
            z[i*8:(i+1)*8] = toarray(tobyte(z[i*8:(i+1)*8]) ^ iv ^ k)
            iv = tobyte(z[i*8:(i+1)*8])
        else:
            c = tobyte(z[i*8:(i+1)*8])
            z[i*8:(i+1)*8] = toarray(c ^ k ^ iv)
            iv = c

    g = np.nditer(z)

    d = np.ndarray(img.shape, dtype=np.bool)

    for i in range(img.shape[0]):
        for j in range(img.shape[1]):
            d[i, j] = next(g)

    return d


# aes cfb encryption
def iaes(img: np.ndarray, key: bytes, encrypt: bool) -> np.ndarray:
    assert len(img.shape) == 2
    assert len(key) == AES.block_size*2 and type(key) == bytes

    z = np.zeros(pad(img.shape[0]*img.shape[1]))

    for i, b in enumerate(np.nditer(img)):
        z[i] = b

    data = bytearray()
    for i in range(len(z)//8):
        data.append(tobyte(z[i*8:(i+1)*8]))

    # iv = b'\x00'*AES.block_size

    iv, k = key[:AES.block_size], key[AES.block_size:]

    cipher = AES.new(k, AES.MODE_CFB, iv)

    if encrypt:
        data = cipher.encrypt(bytes(data))
    else:
        data = cipher.decrypt(bytes(data))

    g = iter(data)

    for i in range(len(z)//8):
        z[i*8:(i+1)*8] = toarray(next(g))

    g = np.nditer(z)

    d = np.ndarray(img.shape, dtype=np.bool)

    for i in range(img.shape[0]):
        for j in range(img.shape[1]):
            d[i, j] = next(g)

    return d
