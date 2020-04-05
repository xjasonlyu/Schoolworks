clear all

I=imread('I.jpg');

h=I*0.43+30;

subplot(1,2,1);
imshow(I);
subplot(1,2,2);
imshow(h);