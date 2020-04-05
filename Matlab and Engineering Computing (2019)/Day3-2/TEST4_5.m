clear all

% TEST 4
I=imread('I.jpg');

subplot(2,2,1);
imshow(I);
subplot(2,2,2);
imshow(I(:,:,1));
subplot(2,2,3);
imshow(I(:,:,2));
subplot(2,2,4);
imshow(I(:,:,3));

% TEST 5
figure;
IG=rgb2gray(I);
imshow(IG);

