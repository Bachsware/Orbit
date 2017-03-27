close all
clear all

addpath '../../Data'
data = load(['MissionAt1.dat']);
        
N_flights = 100;

iSpacecraft = 1;
iSun        = 2;
iEarth      = 3;
iMoon       = 4;
iMars       = 5;

Objs    = [iSpacecraft iSun iMars];
N_obj   = length(Objs);
N_steps = length(data(:,2));

R = zeros(N_steps,3,N_obj,N_flights);
T = zeros(N_steps,N_flights);
disp('Fetching data')
for index = 0:N_flights
    for j = 1:N_obj
        data = load(['MissionAt' num2str(index) '.dat']);
        R(:,1,j,index+1)= data(:,1+(Objs(j)-1)*3+1)./1000;
        R(:,2,j,index+1)= data(:,1+(Objs(j)-1)*3+2)./1000;
        R(:,3,j,index+1)= data(:,1+(Objs(j)-1)*3+3)./1000;
    end
    
    T(:,index+1) = data(:,1);
end

N_steps = length(data(:,2));

T=T/(60*60*24);

figure('Position', [0, 0, 1920, 1080],'Color',[0 0 0])

set(gca,'Color',[0 0 0]);
colors = {'c.','b.','g.','m.','r.',}
legends = {'SpaceCraft','Sun','Earth','Moon','Mars'}


disp('Changing restframe')
for index = 0:N_flights
    for j=1:N_steps
        
        I = iSun;
        
        x0=R(j,1,I,index+1);
        y0=R(j,2,I,index+1); 
        z0=R(j,3,I,index+1);
        
        for k=1:N_obj
            R(j,1,k,index+1) = R(j,1,k,index+1)-x0;
            R(j,2,k,index+1) = R(j,2,k,index+1)-y0;
            R(j,3,k,index+1) = R(j,3,k,index+1)-z0;
        end
    end
end



axis equal
axis tight
hold on
%view([21 12])

dN = 1;

spacecraft  = plot3(2,0,0,'c+','MarkerSize',5);
sun         = plot3(0,0,0,'y.','MarkerSize',50);
earth       = plot3(0,0,0,'g.','MarkerSize',30);
moon        = plot3(0,0,0,'m.','MarkerSize',10);
mars        = plot3(0,2,0,'r.','MarkerSize',30);

obj={spacecraft, sun, mars};

lgd = legend(legends)
lgd.TextColor = 'w';
hold on

disp('Plotting')
for index= 1:N_flights
    disp(['flight' num2str(index)])
    title(['\color{white}Days passed: ' num2str(T(end,index))])
    for k=1:3
        %set(obj{k},'Xdata',R(j+dN,1,k,index),'Ydata',R(j+dN,2,k,index),'Zdata', R(j+dN,3,k,index))

        %plot3([R(j,1,k,index) R(j+dN,1,k,index)],[R(j,2,k,index) R(j+dN,2,k,index)],[R(j,3,k,index) R(j+dN,3,k,index)],colors{k})
        plot3(R(:,1,k,index),R(:,2,k,index),R(:,3,k,index),colors{k})
        hold on
    end
end
hold on