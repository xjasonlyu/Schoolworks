#!/usr/bin/env python3

import os
import cv2

from lsb import lsb_encode
from lsb import lsb_decode

from utils import *

X_SIZE = 512
Y_SIZE = 512


def test_lsb(orig, mark, label=''):
    cap = (mark.shape[0] * mark.shape[1]) / (orig.shape[0] * orig.shape[1])
    orig_enc = lsb_encode(orig, mark)
    mark_dec = lsb_decode(orig_enc, mark.shape)

    # imshow(orig, orig_enc, mark, mark_dec, cols=2,
    #        title=f'Embedding with {cap*100:.2f}% capacity',
    #        titles=[
    #            'Original Image', 'LSB Encoded Image',
    #            'Original Watermark', 'Extracted Watermark'
    #        ])

    # print(f'{label}:\tPSNR of {orig.shape[0]}x{orig.shape[1]} image with {cap*100:.2f}% capacity is {psnr(orig, orig_enc):.4f}')

    # print(f'{label}:\tFalse Rate of original watermark and the extracted watermark is {false_rate(mark, mark_dec)*100:.2f}%')


def test_robustness(orig, mark, f, args):
    cap = (mark.shape[0] * mark.shape[1]) / (orig.shape[0] * orig.shape[1])

    orig_enc = lsb_encode(orig, mark)

    for arg in args:
        orig_enc_robust = f(orig_enc, arg)
        mark_dec_robust = lsb_decode(orig_enc_robust, mark.shape)

        imshow(orig, orig_enc_robust, mark, mark_dec_robust, cols=2, show=True,
               title=f'Robustness test with {f.__name__}',
               titles=[
                   'Original Image', 'Attacked Image',
                   'Original Watermark', 'Extracted Attacked Watermark'
               ])


def draw_robust_graph(orig, mark, f, args, title=''):
    cap = (mark.shape[0] * mark.shape[1]) / (orig.shape[0] * orig.shape[1])

    orig_enc = lsb_encode(orig, mark)

    false_rate_l = np.zeros(len(args))
    psnr_l = np.zeros(len(args))
    orig_img_psnr_l = np.zeros(len(args))

    for i, arg in enumerate(args):
        print(f'\r{arg:.1f}', end='')
        orig_enc_robust = f(orig_enc, arg)
        mark_dec_robust = lsb_decode(orig_enc_robust, mark.shape)
        false_rate_l[i] = np.float(false_rate(mark, mark_dec_robust)*100)
        # psnr_l[i] = np.float(psnr(mark, mark_dec_robust, 1))
        # orig_img_psnr_l[i] = np.float(psnr(orig_enc, orig_enc_robust))

    fig, axs = plt.subplots(1, 2, figsize=(10, 4))
    fig.suptitle(title)

    plt.gray()

    axs[0].imshow(orig_enc)
    axs[0].set_title('LSB embedded image')

    # axs[0, 1].plot(args, orig_img_psnr_l, 'r')
    # axs[0, 1].set_title('PSNR ')
    # axs[0, 1].set(xlabel='σ²', ylabel='peak signal noise ratio')

    axs[1].plot(args, false_rate_l, 'g')
    axs[1].set_title('False Rate Graph')
    # axs[0].grid(True)
    # axs[1].set(xlabel='σ²', ylabel='false rate (%)')
    axs[1].set(xlabel='quality', ylabel='false rate (%)')

    # axs[1, 1].plot(args, psnr_l, 'b')
    # axs[1].set_title('PSNR')
    # axs[1].grid(True)
    # axs[1, 1].set(xlabel='σ²', ylabel='peak signal noise ratio')

    plt.show()


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

    # imshow(decade_256, decade_256_b)

    # original_enc = lsb_encode(original_image, decade_256_b)

    # decade_256_dec = lsb_decode(original_enc, decade_256_b.shape)

    # orig_noisy = gaussian_noise(original_enc, 0.1)

    # decade_256_noisy_dec = lsb_decode(orig_noisy, decade_256_b.shape)

    # orig_compressed = jpeg_compress(original_enc, 98)

    # decade_256_compressed_dec = lsb_decode(orig_compressed, decade_256_b.shape)

    # imshow(original_image, original_enc, orig_noisy, decade_256_b, decade_256_dec, decade_256_noisy_dec, cols=3)
    # imshow(original_enc, orig_noisy, orig_compressed, decade_256_dec, decade_256_noisy_dec, decade_256_compressed_dec, cols=2)

    #print(psnr(original_image, original_enc))
    #print(error_ratio(decade_256_b, decade_256_dec))

    # for orig_img_name in os.listdir('images/original'):
    #     orig_img = cv2.imread(os.path.join('images/original', orig_img_name), cv2.IMREAD_GRAYSCALE)
    #     for test_img in (decade_256_b, kabuto_400_b, kuuga_512_b):
    #         test_lsb(orig_img, test_img, label=orig_img_name)

    #test_robustness(original_image, decade_256_b, gaussian_noise, [0.01, 0.1, 1])

    #draw_robust_graph(original_image, decade_256_b, gaussian_noise, [i*0.001 for i in range(1, 1001, 1)], title='Robustness Test: Gaussian Noise')

    # test_robustness(original_image, decade_256_b, jpeg_compress, [99, 98, 90, 1])

    # draw_robust_graph(original_image, decade_256_b, jpeg_compress, [i for i in range(0, 101, 1)], title='Robustness Test: JPEG Compress')


if __name__ == '__main__':
    test()
