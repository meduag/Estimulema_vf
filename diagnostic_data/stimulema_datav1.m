%% Magnitud fuerzas concorrentes de un acelerometro
clc
close all
clear

%% Cargar Valores de las series temporales
load arq01_cr.txt
df = arq01_cr;
SignalC = df(:,1); % señal de control mA
SignalC(SignalC == 2) = -1;
%SignalC2(SignalC2 == 2) = -1;
mA = df(:,2); % señal de control mA
SignalC2 = df(:,3); % señal de control mA
ejeX = df(:,4);
ejeY = df(:,5);
ejeZ = df(:,6);

%% Normaliza las series temporales 
ejeX = ejeX / max(ejeX); % Normalizo de 0 a 1
if(max(ejeX) > 1)
    ejeX = ejeX / max(ejeX); % Normalizo de 0 a 1
end

ejeY = ejeY / max(ejeY); % Normalizo de 0 a 1
if(max(ejeY) > 1)
    ejeY = ejeY / max(ejeY); % Normalizo de 0 a 1
end

ejeZ = ejeZ / max(ejeZ); % Normalizo de 0 a 1
if(max(ejeZ) > 1)
    ejeZ = ejeZ / max(ejeZ); % Normalizo de 0 a 1
end

%% Grafico los tres ejes
figure();
subplot(311);
plot(ejeX);
subplot(312);
plot(ejeY);
subplot(313);
plot(ejeZ);

%% Grafico los tres ejes
figure();
plot(SignalC);
hold on
plot(ejeX);

%% Grafico los tres ejes
figure();
plot(SignalC2);
hold on
plot(ejeX);
plot(SignalC);
