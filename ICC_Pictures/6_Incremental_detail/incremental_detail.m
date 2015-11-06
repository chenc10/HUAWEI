lineWidth = 1;
markerSize=4;
labelTestSize = 10;
legendTextSize = 10;
boxRatioTextSize = 10;
x=[0,1,2,3,4,5,6,7];
x1=[0 , 0.0 , 9.14264150943 , 66.7381132075 , 97.441509434 , 99.2641509434 , 99.9781132075 , 100.0];
x2=[0 , 14.5051169591 , 79.451754386 , 97.7002923977 , 99.2251461988 , 99.9437134503 , 100.0 , 100.0]; 
x3=[0 , 27.645631068 , 88.8515950069 , 98.8023578363 , 99.5755894591 , 99.9937586685 , 100.0 , 100.0]; 
x4=[0 , 56.1003460208 , 94.8228373702 , 98.9612456747 , 99.8484429066 , 100.0 , 100.0 , 100.0]; 
x5=[0 , 74.6151202749 , 97.4838487973 , 99.0611683849 , 99.9285223368 , 100.0 , 100.0 , 100.0]; 
x6=[0 , 85.158110883 , 98.8802190281 , 99.3880903491 , 100.0 , 100.0 , 100.0 , 100.0]; 


% greedySC = [1014.3301, 775.649, 598.08936, 465.34558];
% randomSC = [479.25082, 316.71838, 264.80457, 217.5541];
% gdvSC = [1618.829, 1067.5519, 758.02277, 590.3807];
% adaptiveSC = [1196.8037, 869.7468, 661.4816, 510.04245];

figure(1);
hold on;
%xSequenceC = {'[10,20]';'[20,30]';'[30,40]';'[40,50]'};
%xSequence = 1:length(greedySC);

a1 = plot(x,x1, 'x-k','LineWidth',lineWidth,'color','b'); M1 = '0';
a2 = plot(x,x2, '^--k','LineWidth',lineWidth,'color','b'); M2 = '1';
a3 = plot(x,x3, 'o-k','LineWidth',lineWidth,'color','r'); M3 = '10';
a4 = plot(x,x4, '*-k','LineWidth',lineWidth,'color','m'); M4 = '100';
a5 = plot(x,x5, '-.k','LineWidth',lineWidth,'color','k'); M5 = '1000';
a6 = plot(x,x6, '+-k','LineWidth',lineWidth,'color','k'); M6 = '8000';
h_legend = legend(M1, M2, M3, M4, M5, M6, 2);
xhand = xlabel('simulating iterations');
yhand = ylabel('average number of ASes already in converged state(%)');
set(a1, 'MarkerSize', markerSize);
set(a2, 'MarkerSize', markerSize);
set(a3, 'MarkerSize', markerSize);
set(a4, 'MarkerSize', markerSize);
set(a5, 'MarkerSize', markerSize);
set(a6, 'MarkerSize', markerSize);
set(h_legend,'FontSize',legendTextSize);
set(h_legend, 'FontWeight', 'bold');
set(gca,'fontsize',boxRatioTextSize);

set(xhand, 'FontSize', labelTestSize);
set(yhand, 'FontSize', labelTestSize);
% set(gca,'XTick',xSequence); % Change x-axis ticks
% set(gca,'XTickLabel',xSequenceC); % Change x-axis ticks labels to desired values.
grid on;
hold off;
