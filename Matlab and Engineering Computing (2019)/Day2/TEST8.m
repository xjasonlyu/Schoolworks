clear all

t=0:0.1:4*pi;
y1=cos(t);
y2=cos(t+0.25);
y3=sin(t+0.5);

subplot(3,1,1);
plot(t,y1,'r.-');
title("cos");
xlabel("t");
ylabel("y");
legend('y1')

subplot(3,1,2);
plot(t,y2,'g+:');
title("cos");
xlabel("t");
ylabel("y");
legend('y2')

subplot(3,1,3);
plot(t,y3,'b*-.');
title("sin");
xlabel("t");
ylabel("y");
legend('y3')