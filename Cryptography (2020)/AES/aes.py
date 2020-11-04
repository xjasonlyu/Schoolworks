import os
import pyaes


def raw_encrypt(key, plaintext):
    assert len(plaintext) == 16

    aes = pyaes.AES(key)
    return aes.encrypt(plaintext)


def raw_decrypt(key, ciphertext):
    assert len(ciphertext) == 16

    aes = pyaes.AES(key)
    return aes.decrypt(ciphertext)


def main():
    key = bytes.fromhex('2b7e151628aed2a6abf7158809cf4f3c')
    msg = bytes.fromhex('6bc1bee22e409f96e93d7e117393172a')

    c = raw_encrypt(key, msg)
    print(bytearray(c).hex())
    # t = raw_decrypt(key, c)
    # print(bytearray(t))


if __name__ == '__main__':
    main()
