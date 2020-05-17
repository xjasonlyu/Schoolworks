#!/usr/bin/env python3

import os
import cv2

from dct import dct_encode
from dct import dct_decode

from utils import *

X_SIZE = 512
Y_SIZE = 512


def test():
    # open base image in grayscale
    original_image = cv2.imread(
        'images/original/boats.bmp', cv2.IMREAD_GRAYSCALE)

    assert original_image.shape == (X_SIZE, Y_SIZE)

    # open watermarks
    faiz_64 = cv2.imread('images/logo/faiz.png', cv2.IMREAD_GRAYSCALE)
    blade_100 = cv2.imread('images/logo/blade.png', cv2.IMREAD_GRAYSCALE)
    zero_one_128 = cv2.imread('images/logo/zero-one.png', cv2.IMREAD_GRAYSCALE)
    decade_256 = cv2.imread('images/logo/decade.png', cv2.IMREAD_GRAYSCALE)
    kabuto_400 = cv2.imread('images/logo/kabuto.png', cv2.IMREAD_GRAYSCALE)
    kuuga_512 = cv2.imread('images/logo/kuuga.png', cv2.IMREAD_GRAYSCALE)

    # convert to binary images
    faiz_64_b = to_binary_image(faiz_64)
    blade_100_b = to_binary_image(blade_100)
    zero_one_128_b = to_binary_image(zero_one_128)
    decade_256_b = to_binary_image(decade_256)
    kabuto_400_b = to_binary_image(kabuto_400)
    kuuga_512_b = to_binary_image(kuuga_512)

    original_enc = dct_encode(original_image, faiz_64_b)
    
    # faiz_64_b_dec = dct_decode(original_image, original_enc, faiz_64_b.shape)
    # noisy = gaussian_noise(original_enc, 0.002)
    noisy = gaussian_noise(original_enc, 0, 0.0002)

    print(original_image, noisy)

    # #imshow(noisy)
    faiz_64_b_dec = dct_decode(original_image, noisy, faiz_64_b.shape)

    imshow(original_image, noisy, faiz_64_b, faiz_64_b_dec, cols=2)

    # original_enc = dct_encode(original_image, blade_100_b)
    # blade_100_b_dec = dct_decode(original_image, original_enc, blade_100_b.shape)

    # imshow(original_image, original_enc, blade_100_b, blade_100_b_dec, cols=2)

    # original_enc = dct_encode(original_image, zero_one_128_b)
    # zero_one_128_b_dec = dct_decode(original_image, original_enc, zero_one_128_b.shape)

    # imshow(original_image, original_enc, zero_one_128_b, zero_one_128_b_dec, cols=2)

    # imshow(original_image, gaussian_noise(original_image, 0.001))


if __name__ == '__main__':
    test()
