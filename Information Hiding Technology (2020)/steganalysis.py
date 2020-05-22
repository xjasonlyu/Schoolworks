#!/usr/bin/env python3

from utils import *

from lsb import lsb_encode
from lsb import lsb_decode

import os
import cv2
import numpy as np
import matplotlib.pyplot as plt
from crypto import ixor, iaes
from scipy.stats import chi2

X_SIZE = 512
Y_SIZE = 512


def get_lsb(img: np.ndarray) -> np.ndarray:
    test = np.ndarray(img.shape, dtype=np.bool)
    for i in range(img.shape[0]):
        for j in range(img.shape[1]):
            test[i, j] = round(img[i, j]) % 2
    return test


def analyze(img: np.ndarray):
    assert len(img.shape) == 2

    m = 4  # split to 4 * 4
    x = img.shape[0] // m  # 512 / 4 = 128
    y = img.shape[1] // m
    temp = np.zeros((x, y))

    r = 0
    k = 0
    hx = np.zeros(x)

    pr = np.zeros(m ** 2)
    pc = np.zeros(m ** 2)

    for p in range(m):
        for q in range(m):
            temp = img[p*x:(p+1)*x, q*y:(q+1)*y]

            #h, _ = np.histogram(temp.ravel(), 256, [0, 256])
            h = cv2.calcHist([temp], [0], None, [256], [0, 256])

            for i in range(x):
                hx[i] = (h[2*i]+h[2*i+1])/2
                if hx[i] != 0:
                    # r += ((h[2*i]-h[2*i+1]) ** 2)/hx[i]
                    r += ((h[2*i]-hx[i]) ** 2)/hx[i]

            # print(r)

            pr[k] = r
            #pc[k] = 1-chi2.pdf(r, 127)
            pc[k] = 1-chi2.cdf(r, 255)

            r = 0
            k += 1

    return pr, pc


def test_results(orig, mark, k, f, s=0.9990, label=''):
    cap = (mark.shape[0] * mark.shape[1]) / (orig.shape[0] * orig.shape[1])

    test_b = mark
    orig_enc = lsb_encode(orig, test_b, k, f)

    pr, pc = analyze(orig)
    result_1 = sum(pc)/len(pc) > s

    print(
        f'original={label}\twatermark={mark.shape[0]}x{mark.shape[1]} \tcapacity={cap*100:.2f}% \twatermarked={False}\tresult={result_1}')

    pr, pc = analyze(orig_enc)
    result_2 = sum(pc)/len(pc) > s

    #print(sum(pc)/len(pc))

    print(
        f'original={label}\twatermark={mark.shape[0]}x{mark.shape[1]} \tcapacity={cap*100:.2f}% \twatermarked={True}\tresult={result_2}')

    return result_1 == True, result_2 == False


def test():
    # open base image in grayscale
    original_image = cv2.imread(
        'images/original/boats.bmp', cv2.IMREAD_GRAYSCALE)

    assert original_image.shape == (X_SIZE, Y_SIZE)

    # open watermarks
    decade_256 = cv2.imread('images/logo/decade.png', cv2.IMREAD_GRAYSCALE)
    kabuto_400 = cv2.imread('images/logo/kabuto.png', cv2.IMREAD_GRAYSCALE)
    kuuga_512 = cv2.imread('images/logo/kuuga.png', cv2.IMREAD_GRAYSCALE)

    # convert to binary images
    decade_256_b = to_binary_image(decade_256)
    kabuto_400_b = to_binary_image(kabuto_400)
    kuuga_512_b = to_binary_image(kuuga_512)

    test_b = kuuga_512_b

    original_enc = lsb_encode(original_image, test_b, os.urandom(32), iaes)

    test_b_dec = lsb_decode(original_enc, test_b.shape, b'\x55\x34'*16, None)

    # imshow(get_lsb(original_image), get_lsb(original_enc), cols=1, titles=[
    #     'LSB of Original Image', 'LSB of Watermarked Image'
    # ])

    pr, pc = analyze(original_enc)

    # print(sum(pc)/len(pc))

    # p = n = 0
    # imgs = os.listdir('images/original')
    # for orig_n in imgs:
    #     orig = cv2.imread('images/original/'+orig_n, cv2.IMREAD_GRAYSCALE)
    #     i, j = test_results(orig, decade_256_b, os.urandom(32), iaes, label=orig_n, s=0.9980)
    #     p += i
    #     n += j

    # print(f'\np={p}, n={n}\nFP={p/(len(imgs)*2)} \t FN={n/(len(imgs)*2)}')

    imshow(pr, pc, mode=1, cols=2, titles=[
        'r = sum(h2i-h2i*)^2/h2i', 'p = 1-chi2.cdf(r, 255)'
    ])

    # from crypto import ixor, iaes

    # i = iaes(decade_256_b, b'x'*32, 1)
    # j = iaes(i, b't'*32, 0)

    # imshow(decade_256_b, i, j)


if __name__ == '__main__':
    test()
