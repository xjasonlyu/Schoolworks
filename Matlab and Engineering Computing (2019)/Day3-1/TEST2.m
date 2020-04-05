clear all

[x,y]=meshgrid(1:100,1:100);
z=x.^2+y.^2;

surf(x,y,z);