close all
clear all

addpath '../../Data'

data = load('OptimizedSpacecraft4.dat');


N_obj   = 5;
N_steps = length(data(:,1));

T = data(:,1);
T=T/(60*60*24);
R = zeros(N_steps,3,N_obj);

iSun    = 2;
iEarth  = 3;
iMoon   = 4;
iMars   = 5;

for j = 1:N_obj
    R(:,1,j)= data(:,1+(j-1)*3+1)./1000;
    R(:,2,j)= data(:,1+(j-1)*3+2)./1000;
    R(:,3,j)= data(:,1+(j-1)*3+3)./1000;
end


figure('Position', [0, 0, 1080, 1080],'Color',[0 0 0])

set(gca,'Color',[0 0 0]);
colors = {'w-','b--','g--','m--','c--',}
legends = {'SpaceCraft','Sun','Earth','Moon','Mars'}


for j=1:N_steps
    I = iSun;
    x0=R(j,1,I); y0=R(j,2,I); z0=R(j,3,I);
    for k=1:N_obj
        R(j,1,k) = R(j,1,k)-x0;
        R(j,2,k) = R(j,2,k)-y0;
        R(j,3,k) = R(j,3,k)-z0;
    end
end

axis equal
axis tight
hold on
view([21 12])
dN = 10
for j=1:dN:N_steps
    title(['\color{white}Days passed: ' num2str(T(j))])
    for k=1:N_obj
        plot3([R(j,1,k) R(j+dN,1,k)],[R(j,2,k) R(j+dN,2,k)],[R(j,3,k) R(j+dN,3,k)],colors{k})
    end
    pause(0.01)
end
hold on
legend(legends)