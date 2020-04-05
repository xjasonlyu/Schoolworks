clear all

a=[1 2 3; -1 3 7; 1 0 3];
b=[1;4;7];

% Method 1
x=a\b;
disp(x);

% Method 2
x=inv(a)*b;
disp(x);