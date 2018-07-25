clc
clear 
close all
 
%% load
 
% accel
load 'acel.txt'
 
accel_t0 = 1e-6 * acel(1,1); % in [s]
accel_N = length(acel(:,1)); 
accel_tf =1e-6 * acel(accel_N,1); % in [s]
 
accel_Ts = (accel_tf - accel_t0) / accel_N; % in [s]
accel_fs = 1/accel_Ts;
accel_t = (accel_t0+accel_Ts:accel_Ts:accel_tf);
 
accel_x_bits = acel(:,2); 
accel_y_bits = acel(:,3);
accel_z_bits = acel(:,4);
 
% stim
load 'stim.txt'
 
stim_t0 = 1e-6 * stim(1,3);
stim_lastLine = length(stim(:,3));
stim_tf = 1e-6 * stim(stim_lastLine,3);
 
stim_t = (stim_t0+accel_Ts:accel_Ts:stim_tf);
stim_N = length(stim_t);
 
accelstim_delay = stim_N - accel_N;
stim_t = stim_t(accelstim_delay+1:stim_N);
stim_N = length(stim_t);
 
accelstim_startdelay = stim_t(1) - accel_t(1);
 
stim_i = acel(:,5);
 
%% convert accel
 
 
%% build stim
 
stim_pulse_counter = 1;
thisstim = 0;
thisvalue = 0;
 
for k = 1:accel_N
     
    if (acel(k,5) - thisstim) > .5
        % new pulse found!
        thisvalue = mean( stim((6*stim_pulse_counter) - 6 + 2:(6*stim_pulse_counter) + 1 , 2));
        thisstim = 1;
        stim_pulse_counter = stim_pulse_counter + 1;
    elseif (thisstim - acel(k,5)) > .5
        thisstim = 0;
    end
     
    stim_i (k) = stim_i (k) *  thisvalue;
     
end
 
%% plot
 
figure;
subplot(211); 
plot(accel_t,accel_x_bits,'r'); 
hold on; plot(accel_t,accel_y_bits,'b'); 
plot(accel_t,accel_z_bits,'g'); 
axis([accel_t0 accel_tf -20000 20000])

subplot(212); plot(accel_t,stim_i);
axis([accel_t0 accel_tf 0 5])