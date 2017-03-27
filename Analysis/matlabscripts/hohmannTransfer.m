close all
clear all

addpath '../../Data'

data = load('OptimizedSpacecraft4.dat');
data = load('OptimizedFuelSpacecraft.dat');


N_obj   = 5;
N_steps = length(data(:,1));

T = data(:,1);
T=T/(60*60*24);
R = zeros(N_steps,3,N_obj);

iSun    = 2;
iEarth  = 3;
iMoon   = 4;
iMars   = 5;


mu_earth = 398600441800000.0;
mu_sun   = 1.327124*1e20;
mu_mars   = mu_earth*0.107;
mu_moon  = 4.902798882*1e12;

for j = 1:N_obj
    R(:,1,j)= data(:,1+(j-1)*3+1);
    R(:,2,j)= data(:,1+(j-1)*3+2);
    R(:,3,j)= data(:,1+(j-1)*3+3);
end

for j=1:N_steps
    I = iSun;
    x0=R(j,1,I); y0=R(j,2,I); z0=R(j,3,I);
    for k=1:N_obj
        R(j,1,k) = R(j,1,k)-x0;
        R(j,2,k) = R(j,2,k)-y0;
        R(j,3,k) = R(j,3,k)-z0;
    end
end

r_earth = R(1,:,1);
r_mars = R(1,:,iMars)
ra = norm(r_earth) + 35786000
rb = norm(r_mars)
mu = mu_sun

a = (ra+rb)/2

VIA = sqrt(mu/ra)
VJB = sqrt(mu/rb);

VaA = sqrt(mu*(2/ra-1/a))
VaB = sqrt(mu*(2/rb-1/a))

dVA = abs(VaA-VIA)
dVB = abs(VaB-VJB)

dV  = dVA + dVB

T   = sqrt(4*pi^2*a^3/mu/2);

timeInDays = T/3600/24

dVAinKmS = dVA/1000
dVBinKmS = dVB/1000
dVinKmS = dV/1000



