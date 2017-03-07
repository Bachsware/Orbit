close all
data = load('out.dat');

T = data(:,1);
X = data(:,2)./1000;
Y = data(:,3)./1000;
Z = data(:,4)./1000;
X2 = data(:,5)./1000 - X;
Y2 = data(:,6)./1000 - Y;
Z2 = data(:,7)./1000 - Z;

%X=X-X;
%Y=Y-Y;
%Z=Z-Z;
w=figure

set(gca,'Color',[0.8 0.8 0.8]);
hold on
%% generate coordinates for sphere
r_earth = 6378000; % earth radius
[xx yy zz] = sphere;

%% now draw sphere with earth radius in terms of km.
s2=surf(xx*r_earth/1000, yy*r_earth/1000, zz*r_earth/1000);
    %% fancy colour map
    set(s2, 'CData', rand(21), 'FaceColor', 'interp')
    % fluffy cloud texture
    colormap(winter) % blue and green colours for the earth
    
%% Animate
axis equal
view([155 20])
axis([-1e4 1e4 -1e4 1e4 -1e4 -1e4])
for i = 1:length(X)-1
    surf(xx*r_earth/1000, yy*r_earth/1000, zz*r_earth/1000);
    hold on
    plot3([X(i) X(i+1)],[Y(i) Y(i+1)],[Z(i) Z(i+1)],'k--')
    plot3([X2(i) X2(i+1)],[Y2(i) Y2(i+1)],[Z2(i) Z2(i+1)],'r--')
    hold off
    pause(0.1)
end