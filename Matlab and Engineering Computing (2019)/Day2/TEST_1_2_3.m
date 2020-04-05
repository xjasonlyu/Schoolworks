clear all

%===TEST 1===%
A=zeros(4,5);
disp(A);
B=ones(4,5);
disp(B);
% random matrix
C=rand(4,5);
disp(C);

%===TEST 2===%
% columns
a=mean(C,1);
disp(a);
% rows
b=mean(C,2);
disp(b);
% all
c=mean(mean(C));
disp(c);

%===TEST 3===%
s=find(C<0.5);
C(s)=0;
s=find(C>=0.5);
C(s)=1;
disp(C);