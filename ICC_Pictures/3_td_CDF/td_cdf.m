lineWidth = 1;
markerSize=2;
labelTestSize = 10;
legendTextSize = 25;
boxRatioTextSize = 10;


x=[4 , 5 , 6 , 7 , 8 ];
y=[1.92696791598e-05 , 0.282898159746 , 0.956142210232 , 0.999479718663 , 1.0]

figure(1);
hold on;


a1 = plot(x, y,'o-k','LineWidth',lineWidth,'color','b');
line([5.76,5.76],[0,1])
xhand = xlabel('\textit{global convergence delay}','Interpreter','LaTex');
yhand = ylabel('CDF of ASes');
set(a1, 'MarkerSize', markerSize);

set(gca,'fontsize',boxRatioTextSize)
set(xhand, 'FontSize', labelTestSize);
set(yhand, 'FontSize', labelTestSize);
set(gca,'XTick',[4,5,6,7,8]); % Change x-axis ticks
set(gca,'XTickLabel',[4,5,6,7,8]); % Change x-axis ticks labels to desired values.
grid on;
% box on;
hold off;
