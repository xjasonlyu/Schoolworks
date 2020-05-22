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

    #mark_dec[1:6,1:6] = 0

    # imshow(orig, orig_enc, mark, mark_dec, cols=2,
    #        title=f'Embedding with {cap*100:.2f}% capacity',
    #        titles=[
    #            'Original Image', 'LSB Encoded Image',
    #            'Original Watermark', 'Extracted Watermark'
    #        ])

    #print(f'original={label}({orig.shape[0]}x{orig.shape[1]})\talpha={alpha} \twatermark={mark.shape[0]}x{mark.shape[1]} \tcapacity={cap*100:.2f}%\tPSNR={psnr(orig, orig_enc):e}')

    print(
        f'original={label}({orig.shape[0]}x{orig.shape[1]})\talpha={alpha} \twatermark={mark.shape[0]}x{mark.shape[1]} \tcapacity={cap*100:.2f}%\tfalse_rate={false_rate(mark, mark_dec)*100:e}%')


def test_robustness(orig, mark, f, args, alpha=5):
    cap = (mark.shape[0] * mark.shape[1]) / (orig.shape[0] * orig.shape[1])

    orig_enc = dct_encode(orig, mark, alpha=alpha)

    for arg in args:
        orig_enc_robust = f(orig_enc, *arg)
        mark_dec_robust = dct_decode(orig, orig_enc_robust, mark.shape)

        imshow(orig, orig_enc_robust, mark, mark_dec_robust, cols=2,
               title=f'Robustness test with {f.__name__} {arg}',
               titles=[
                   'Original Image', 'Attacked Image',
                   'Original Watermark', 'Extracted Attacked Watermark'
               ])


def draw_gaussian_robust_graph(orig, mark, args):
    cap = (mark.shape[0] * mark.shape[1]) / (orig.shape[0] * orig.shape[1])

    # alpha = 5
    orig_enc = dct_encode(orig, mark, alpha=5)

    false_rate_l_5 = np.zeros(len(args))
    for i, arg in enumerate(args):
        print(f'\ralpha={5} {arg:.5f}', end='')
        orig_enc_robust = gaussian_noise(orig_enc, m=0, v=arg)
        mark_dec_robust = dct_decode(orig, orig_enc_robust, mark.shape)
        false_rate_l_5[i] = np.float(false_rate(mark, mark_dec_robust)*100)

    # alpha = 10
    orig_enc = dct_encode(orig, mark, alpha=10)

    false_rate_l_10 = np.zeros(len(args))
    for i, arg in enumerate(args):
        print(f'\ralpha={10} {arg:.5f}', end='')
        orig_enc_robust = gaussian_noise(orig_enc, m=0, v=arg)
        mark_dec_robust = dct_decode(orig, orig_enc_robust, mark.shape)
        false_rate_l_10[i] = np.float(false_rate(mark, mark_dec_robust)*100)

    # alpha = 20
    orig_enc = dct_encode(orig, mark, alpha=20)

    false_rate_l_20 = np.zeros(len(args))
    for i, arg in enumerate(args):
        print(f'\ralpha={20} {arg:.5f}', end='')
        orig_enc_robust = gaussian_noise(orig_enc, m=0, v=arg)
        mark_dec_robust = dct_decode(orig, orig_enc_robust, mark.shape)
        false_rate_l_20[i] = np.float(false_rate(mark, mark_dec_robust)*100)

    #print(list(zip(args, false_rate_l)))

    #assert len(false_rate_l) == len(args)

    fig = plt.figure()
    plt.plot(args, false_rate_l_5, label='Alpha=5')
    plt.plot(args, false_rate_l_10, label='Alpha=10')
    plt.plot(args, false_rate_l_20, label='Alpha=20')

    #fig.suptitle('Robustness Test: Gaussian Noise')

    plt.title('False Rate with Different Alpha')
    plt.xlabel('Gaussian Noise')
    plt.ylabel('False Rate (%)')

    plt.legend(loc="lower right")

    plt.show()


def draw_jpeg_robust_graph(orig, mark, args):
    cap = (mark.shape[0] * mark.shape[1]) / (orig.shape[0] * orig.shape[1])

    # alpha = 5
    orig_enc = dct_encode(orig, mark, alpha=5)

    false_rate_l_5 = np.zeros(len(args))
    for i, arg in enumerate(args):
        print(f'\ralpha={5} {arg}', end='')
        orig_enc_robust = jpeg_compress(orig_enc, quality=arg)
        mark_dec_robust = dct_decode(orig, orig_enc_robust, mark.shape)
        false_rate_l_5[i] = np.float(false_rate(mark, mark_dec_robust)*100)
    print()

    # alpha = 10
    orig_enc = dct_encode(orig, mark, alpha=10)

    false_rate_l_10 = np.zeros(len(args))
    for i, arg in enumerate(args):
        print(f'\ralpha={10} {arg}', end='')
        orig_enc_robust = jpeg_compress(orig_enc, quality=arg)
        mark_dec_robust = dct_decode(orig, orig_enc_robust, mark.shape)
        false_rate_l_10[i] = np.float(false_rate(mark, mark_dec_robust)*100)
    print()

    # alpha = 20
    orig_enc = dct_encode(orig, mark, alpha=20)

    false_rate_l_20 = np.zeros(len(args))
    for i, arg in enumerate(args):
        print(f'\ralpha={20} {arg}', end='')
        orig_enc_robust = jpeg_compress(orig_enc, quality=arg)
        mark_dec_robust = dct_decode(orig, orig_enc_robust, mark.shape)
        false_rate_l_20[i] = np.float(false_rate(mark, mark_dec_robust)*100)
    print()

    #print(list(zip(args, false_rate_l)))

    #assert len(false_rate_l) == len(args)

    fig = plt.figure()
    plt.plot(args, false_rate_l_5, label='Alpha=5')
    plt.plot(args, false_rate_l_10, label='Alpha=10')
    plt.plot(args, false_rate_l_20, label='Alpha=20')

    #fig.suptitle('Robustness Test: Gaussian Noise')

    plt.title('False Rate with Different Alpha')
    plt.xlabel('JPEG Compress Quality')
    plt.ylabel('False Rate (%)')

    plt.legend(loc="upper right")

    plt.show()


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

    # for orig_n in ['boats.bmp', 'peppers.bmp']:
    #     orig = cv2.imread('images/original/'+orig_n,
    #                               cv2.IMREAD_GRAYSCALE)
    #     for mark in [faiz_64_b, blade_100_b, zero_one_128_b]:
    #         for a in [1, 3, 5, 10, 20]:
    #             test_dct(orig, mark, alpha=a, label=orig_n)
    #         print()

    # test_robustness(original_image, zero_one_128_b, gaussian_noise, [(0,0.01)], alpha=20)

    # test_robustness(original_image, zero_one_128_b, jpeg_compress, [[95], [75], [40], [20]], alpha=5)
    # test_robustness(original_image, zero_one_128_b, jpeg_compress, [[40], [20]], alpha=20)
    # test_robustness(original_image, zero_one_128_b, jpeg_compress, [[5]], alpha=150)

    #draw_gaussian_robust_graph(original_image, zero_one_128_b, [(i)*0.00001 for i in range(10)]+[(i)*0.0001 for i in range(10) if i] + [(i)*0.001 for i in range(5) if i])

    #draw_jpeg_robust_graph(original_image, zero_one_128_b, list(range(5,101,5)))

    # original_enc = dct_encode(original_image, blade_100_b)
    # blade_100_b_dec = dct_decode(original_image, original_enc, blade_100_b.shape)

    # imshow(original_image, original_enc, blade_100_b, blade_100_b_dec, cols=2)

    # original_enc = dct_encode(original_image, zero_one_128_b)
    # zero_one_128_b_dec = dct_decode(original_image, original_enc, zero_one_128_b.shape)

    # imshow(original_image, original_enc, zero_one_128_b, zero_one_128_b_dec, cols=2)

    # imshow(original_image, gaussian_noise(original_image, 0.001))


if __name__ == '__main__':
    test()
