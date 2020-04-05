clear all

rows=20;
cols=30;
A=rand(rows,cols);
disp(A);

A(1:2:rows,:)=0;
A(2:2:rows,:)=1;
disp(A);