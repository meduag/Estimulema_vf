clc
clear 
 
% accel
load 'acel_c.txt'
df = acel_c;
SignalC = df(:,1); % se�al de control mA
ejeX = df(:,2);
ejeY = df(:,3);
ejeZ = df(:,4);

figure()
plot(ejeX)
hold on
plot(ejeY)
plot(ejeZ)

