#!/usr/bin/env python3

import cv2
import numpy as np

BLK_SIZE = 8

BLOCK_SAMPLE = [[0, 0, 0, 0, 1, 1, 0, 0],
                [0, 0, 0, 1, 1, 0, 0, 0],
                [0, 0, 1, 1, 0, 0, 0, 0],
                [0, 1, 1, 0, 0, 0, 0, 0],
                [1, 1, 0, 0, 0, 0, 0, 0],
                [0, 0, 0, 0, 0, 0, 0, 0],
                [0, 0, 0, 0, 0, 0, 0, 0],
                [0, 0, 0, 0, 0, 0, 0, 0]]


def dct_encode(orig, mark, alpha=5):
    # check
    assert len(orig.shape) == 2 and len(mark.shape) == 2
    assert orig.shape[0] >= mark.shape[0] and orig.shape[1] >= mark.shape[1]
    assert orig.shape[0] == orig.shape[1] and mark.shape[0] == mark.shape[1]

    # image size
    x, y = orig.shape

    # watermark bits
    g = np.nditer(mark)
    flag = False

    orig_f = np.float32(orig.copy())
    base_f = orig_f.copy()

    for i in range(x//BLK_SIZE):
        for j in range(y//BLK_SIZE):
            block = orig_f[i*BLK_SIZE:i*BLK_SIZE +
                           BLK_SIZE, j*BLK_SIZE:j*BLK_SIZE+BLK_SIZE]
            # dct encode
            block_dct = cv2.dct(block)

            try:
                for x in range(4):
                    bit = int(next(g))
                    assert bit in (0, 1)

                    offset = 1 if bit else -1
                    block_dct[x*2:x*2+2, 6-x*2:8-x*2] += alpha*offset
                    # block_dct[0:2, 6:8] += alpha*offset
                    # block_dct[2:4, 4:6] += alpha*offset
                    # block_dct[4:6, 2:4] += alpha*offset
                    # block_dct[6:8, 0:2] += alpha*offset
            except StopIteration:
                flag = True

            # inverse dct
            block = cv2.idct(block_dct)

            base_f[i*BLK_SIZE:i*BLK_SIZE +
                   BLK_SIZE, j*BLK_SIZE:j*BLK_SIZE+BLK_SIZE] = block

            if flag:
                break
        else:
            continue
        break

    return np.uint8(base_f)


def dct_decode(orig, base, shape):
    assert len(shape) == 2

    x, y = orig.shape

    count = 0
    total = shape[0] * shape[1]
    flag = False
    bits = []

    orig_f = np.float32(orig)
    base_f = np.float32(base)

    # print(np.mean(base_f), np.mean(orig_f))

    mark = np.zeros(shape)

    for i in range(x//BLK_SIZE):
        for j in range(y//BLK_SIZE):
            orig_block = orig_f[i*BLK_SIZE:i*BLK_SIZE +
                                BLK_SIZE, j*BLK_SIZE:j*BLK_SIZE+BLK_SIZE]
            base_block = base_f[i*BLK_SIZE:i*BLK_SIZE +
                                BLK_SIZE, j*BLK_SIZE:j*BLK_SIZE+BLK_SIZE]
            # dct encode
            orig_block_dct = cv2.dct(orig_block)
            base_block_dct = cv2.dct(base_block)

            for x in range(4):
                if count > total:
                    flag = True
                    break
                bits.append(1 if np.mean(base_block_dct[x*2:x*2+2, 6-x*2:8-x*2]) > np.mean(
                    orig_block_dct[x*2:x*2+2, 6-x*2:8-x*2]) else 0)
                count += 1

            if flag:
                break
        else:
            continue
        break

    g = (int(x) for x in bits)
    for i in range(shape[0]):
        for j in range(shape[1]):
            try:
                mark[i, j] = next(g)
            except StopIteration:
                break
        else:
            continue
        break

    return mark
