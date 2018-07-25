%% Magnitud fuerzas concorrentes de un acelerometro
clc
close all
clear

%% Cargar Valores de las series temporales
load time.txt;
df = time;
s = df(:,1);
x = df(:,2);
y = df(:,3);
z = df(:,4);
t = df(:,5);