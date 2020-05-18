#!/usr/bin/env python3

import os
import math
from io import BytesIO

import cv2
import numpy as np
import matplotlib.pyplot as plt

from skimage.util import random_noise
from skimage.metrics import peak_signal_noise_ratio


def to_binary_image(img):
    _, b = cv2.threshold(img, 127, 1, cv2.THRESH_BINARY)
    return b


def psnr(img, test, r=None):
    mse = np.mean((img - test) ** 2)
    if(mse == 0):
        return 100.0
    # max_pixel = 255.0
    # return 20 * math.log10(max_pixel / math.sqrt(mse))
    # as alias
    return peak_signal_noise_ratio(img, test, data_range=r)


def gaussian_noise(img, m, v):
    noisy = random_noise(np.uint8(img), mode='gaussian',
                         seed=None, mean=m, var=v)
    return np.uint8(noisy*255)
    # mean = 0
    # sigma = math.sqrt(n)
    # gauss = np.random.normal(mean, sigma, img.shape)
    # noisy = img.copy() + gauss.reshape(*img.shape)
    # return noisy


def jpeg_compress(img, quality=95):
    _tmp_img = '.tmp/__tmp_compressed.jpg'
    cv2.imwrite(_tmp_img, img, [
                int(cv2.IMWRITE_JPEG_QUALITY), quality])
    assert os.path.exists(_tmp_img)
    return cv2.imread(_tmp_img, cv2.IMREAD_GRAYSCALE)


def false_rate(base, test):
    assert len(base.shape) == 2 and base.shape == test.shape
    count = 0
    total = base.shape[0] * base.shape[1]
    # iterate
    for p, q in zip(np.nditer(base), np.nditer(test)):
        # binary images validation check
        assert p in (0, 1) and q in (0, 1)
        if p != q:
            count += 1

    return count / total


def imshow(*images, cols=1, x=8, y=7, show=True, title=None, titles=None):
    assert titles is None or len(images) == len(titles)

    n = len(images)
    fig = plt.figure(title, figsize=(x, y))
    # if title:
    #     fig.suptitle(title, fontsize=16)

    for i, image in enumerate(images):
        a = fig.add_subplot(cols, np.ceil(n/float(cols)), i + 1)
        if image.ndim == 2:
            plt.gray()
        plt.imshow(image)
        if titles is not None:
            a.set_title(titles[i])
    # fig.set_size_inches(np.array(fig.get_size_inches()) * n_images)
    if show:
        plt.show()
