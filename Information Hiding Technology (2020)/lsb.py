#!/usr/bin/env python3

import numpy as np

from crypto import *


def lsb_encode(orig, mark, k=None, f: Callable = ixor):
    # check
    assert len(orig.shape) == 2
    assert orig.shape[0] >= mark.shape[0] and orig.shape[1] >= mark.shape[1]

    base = orig.copy()

    temp = mark if not k or not f else f(mark, k, True)

    # do lsb
    for i in range(temp.shape[0]):
        for j in range(temp.shape[1]):
            d = base[i, j]
            s = temp[i, j]
            # check
            assert s in (0, 1)
            # process
            if d % 2 == s:
                continue
            base[i, j] = ((d >> 1) << 1) + s

    return base


def lsb_decode(base, shape, k=None, f: Callable = ixor):
    mark = np.ndarray(shape, dtype=np.bool)
    for i in range(shape[0]):
        for j in range(shape[1]):
            mark[i, j] = round(base[i, j]) % 2

    return mark if not k or not f else f(mark, k, False)
