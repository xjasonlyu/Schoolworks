clear all

x=[1 1.2 1.4 1.6 1.8];
y=[1 1.0954 1.1832 1.2649 1.3416];

p=polyfit(x,y,4);

poly2str(p,'x')