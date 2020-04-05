clear all

% TEST 1
I=imread('I.jpg');

disp(size(I));

% TEST 2
imshow(I);

% TEST 3
imwrite(I, 'I.bmp');