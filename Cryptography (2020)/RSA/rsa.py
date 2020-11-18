#!/usr/bin/env python3
import os
from Cryptodome.Util.number import getPrime, isPrime

E_SIZE = 64
KEY_SIZE = 128


def int_to_bytes(i):
    return i.to_bytes((i.bit_length() + 7) // 8, 'big')


def int_from_bytes(b):
    return int.from_bytes(b, 'big')


def egcd(a, b):
    if a == 0:
        return b, 0, 1

    g, y, x = egcd(b % a, a)
    return g, x - (b // a) * y, y


def modular_inverse(e, t):
    g, x, _ = egcd(e, t)

    assert g == 1
    return x % t


def generate_keypair(p, q):
    assert p != q
    assert isPrime(p) and isPrime(q)

    n = p * q

    phi = (p-1) * (q-1)

    e = getPrime(E_SIZE)
    assert e < phi

    # e & phi co-prime
    g, _, _ = egcd(e, phi)
    assert g == 1

    d = modular_inverse(e, phi)

    # Public key: (n, e), Private key: (n, d)
    return ((n, e), (n, d))


def encrypt(public_key, plaintext):
    assert len(public_key) == 2

    n, e = public_key

    m = int_from_bytes(plaintext)

    assert m < n
    g, _, _ = egcd(m, n)
    assert g == 1

    c = pow(m, e, n)

    return int_to_bytes(c)


def decrypt(private_key, ciphertext):
    assert len(private_key) == 2

    n, d = private_key

    c = int_from_bytes(ciphertext)
    m = pow(c, d, n)

    return int_to_bytes(m)


def main():
    p = getPrime(KEY_SIZE)
    q = getPrime(KEY_SIZE)
    print(f'p: {p}, q: {q}')

    print()

    public_key, private_key = generate_keypair(p, q)
    print(f'pub_key: {public_key}')
    print(f'pri_key: {private_key}')

    print()

    msg = b'hello world!'

    ciphertext = encrypt(public_key, msg)
    print(f'ciphertext: {ciphertext}')

    plaintext = decrypt(private_key, ciphertext)
    print(f'plaintext: {plaintext}')


if __name__ == '__main__':
    main()
