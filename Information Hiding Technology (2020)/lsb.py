#!/usr/bin/env python3

import numpy as np


def lsb_encode(orig, mark):
    # check
    assert len(orig.shape) == 2
    assert orig.shape[0] >= mark.shape[0] and orig.shape[1] >= mark.shape[1]

    base = orig.copy()
    # do lsb
    for i in range(mark.shape[0]):
        for j in range(mark.shape[1]):
            d = base[i, j]
            s = mark[i, j]
            # check
            assert s in (0, 1)
            # process
            if d % 2 == s:
                continue
            base[i, j] = ((d >> 1) << 1) + s

    return base


def lsb_decode(base, shape):
    mark = np.zeros(shape)
    x, y = shape
    for i in range(x):
        for j in range(y):
            mark[i, j] = round(base[i, j]) % 2

    return mark
