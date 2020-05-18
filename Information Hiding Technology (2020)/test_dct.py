#!/usr/bin/env python3

import os
import cv2

from dct import dct_encode
from dct import dct_decode

from utils import *

X_SIZE = 512
Y_SIZE = 512


def test_dct(orig, mark, alpha, label=''):
    cap = (mark.shape[0] * mark.shape[1]) / (orig.shape[0] * orig.shape[1])
    orig_enc = dct_encode(orig, mark, alpha=alpha)
    mark_dec = dct_decode(orig, orig_enc, mark.shape)

    # imshow(orig, orig_enc, mark, mark_dec, cols=2,
    #        title=f'Embedding with {cap*100:.2f}% capacity',
    #        titles=[
    #            'Original Image', 'LSB Encoded Image',
    #            'Original Watermark', 'Extracted Watermark'
    #        ])

    print(f'original={label}({orig.shape[0]}x{orig.shape[1]})\talpha={alpha}\twatermark={mark.shape[0]}x{mark.shape[1]}\tcapacity={cap*100:.2f}%\tPSNR={psnr(orig, orig_enc):.4f}')



def test():
    # open base image in grayscale
    original_image = cv2.imread(
        'images/original/lena.bmp', cv2.IMREAD_GRAYSCALE)

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


    # test_logo = zero_one_128_b
    # original_enc = dct_encode(original_image, test_logo)
    
    # # faiz_64_b_dec = dct_decode(original_image, original_enc, faiz_64_b.shape)
    # # noisy = gaussian_noise(original_enc, 0.002)
    # #noisy = gaussian_noise(original_enc, 0, 0.0002)
    # #noisy = jpeg_compress(original_enc, quality=80)
    # noisy = original_enc

    # #print(original_image, noisy)

    # # #imshow(noisy)
    # test_dec = dct_decode(original_image, noisy, test_logo.shape)

    # imshow(original_image, noisy, test_logo, test_dec, cols=2, titles=[
    #     'Original Image', 'DCT Embedded Image',
    #     'Original Watermark', 'Extracted Watermark'
    # ])

    for orig_n in ['lena.bmp', 'boats.bmp']:
        for mark in [faiz_64_b, blade_100_b, zero_one_128_b]:
            for a in range(1, 6):
                orig = cv2.imread('images/original/'+orig_n, cv2.IMREAD_GRAYSCALE)
                test_dct(orig, mark, alpha=a, label=orig_n)
            print()

    # original_enc = dct_encode(original_image, blade_100_b)
    # blade_100_b_dec = dct_decode(original_image, original_enc, blade_100_b.shape)

    # imshow(original_image, original_enc, blade_100_b, blade_100_b_dec, cols=2)

    # original_enc = dct_encode(original_image, zero_one_128_b)
    # zero_one_128_b_dec = dct_decode(original_image, original_enc, zero_one_128_b.shape)

    # imshow(original_image, original_enc, zero_one_128_b, zero_one_128_b_dec, cols=2)

    # imshow(original_image, gaussian_noise(original_image, 0.001))


if __name__ == '__main__':
    test()
