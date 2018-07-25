%% Magnitud fuerzas concorrentes de un acelerometro
clc
close all
clear

%% Cargar Valores de las series temporales
%load time.txt;
%df = time;
load stim.txt;
df = stim;
s = df(:,1);
s(s == 2) = -1;
m = df(:,2);
t = df(:,3);

%% figuras primera captura
%figure()
%plot(s);
%figure()
%plot(m);
%figure()
%plot(t);

%% segunda captura
load acel.txt;
df2 = acel;
%s2 = df2(:,1);
%s2(s2 == 2) = -1;
%m2 = df2(:,2);
tp = df2(:,1);
x = df2(:,2);
y = df2(:,3);
z = df2(:,4);
ts = df2(:,5);

yb = true;
xb = true;

figure();
plot(ts);
ind = zeros(max(m) * 2,1);
cl = 1;
for c = 1:length(ts)
    if ts(c) == 1 && ts(c+1) == 1 && yb == true
        disp(c)
        ind(cl,1) = c;
        cl = cl + 1;
        yb = false;
    end
    if ts(c) == 1 && ts(c+1) == 0 && yb == false
        disp(c)
        ind(cl) = c;
        cl = cl + 1;
        yb = true;
    end
end


disp(ind)

%% Normaliza las series temporales 
x = x / max(x); % Normalizo de 0 a 1
if(max(x) > 1)
    x = x / max(x); % Normalizo de 0 a 1
end

y = y / max(y); % Normalizo de 0 a 1
if(max(y) > 1)
    y = y / max(y); % Normalizo de 0 a 1
end

z = z / max(z); % Normalizo de 0 a 1
if(max(z) > 1)
    z = z / max(z); % Normalizo de 0 a 1
end

%% figuras segunda captura
figure();
subplot(311);
plot(x);
subplot(312);
plot(y);
subplot(313);
plot(z);

figure();
plot(ts);

%% tiempo primera captura
t1 = t(1);
tt = length(t);
t2 = t(tt);
tf = t2-t1

%% tiempo segunda captura
p1 = tp(1);
pt = length(tp);
p2 = tp(pt);
pf = p2-p1

%% Diferencia
diferencia = tf - pf
