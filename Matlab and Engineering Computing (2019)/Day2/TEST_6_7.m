clear all

%===TEST 6===%
t=0:0.1:4*pi;
y1=cos(t);
plot(t,y1,'r.-');
hold on;

y2=cos(t+0.25);
plot(t,y2,'g+:');
hold on;

y3=sin(t+0.5);
plot(t,y3,'b*-.');

%===TEST 7===%
title("MATLAB");
xlabel("t");
ylabel("y");
text(0,1,"(0,1)");
legend('y1')