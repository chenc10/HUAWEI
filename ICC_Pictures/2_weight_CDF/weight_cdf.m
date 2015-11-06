

lineWidth = 1;
markerSize=2;
labelTestSize = 10;
legendTextSize = 25;
boxRatioTextSize = 10;

% x=[1, 2, 4, 13, 66, 331, 1656, 8292, 41562, 208136, 1042101, 5429595, 23124464];
% y=[0.1511320936506407, 0.36689469120339147, 0.5024568840928798, 0.6056074766355141, 0.6994893535022642, 0.7495327102803738, 0.7976490991424993, 0.8972348010405626, 0.9803063878986414, 0.996589266788708, 0.9995375277001638, 0.9998458425667213, 1.0];
x=[1, 2, 4, 13, 66, 331, 1656, 8292, 41562, 208136, 1042101];
y=[0.1511320936506407, 0.36689469120339147, 0.5024568840928798, 0.6056074766355141, 0.6994893535022642, 0.7495327102803738, 0.7976490991424993, 0.8972348010405626, 0.9803063878986414, 0.996589266788708, 0.9995375277001638];

figure(1);
hold on;


a1 = plot(x, y,'o-k','LineWidth',lineWidth,'color','b');
%line([5.76,5.76],[0,1])
xhand = xlabel('\textit{neighborhood weight}', 'Interpreter','LaTex');
%  xlabel('$Time$','Interpreter','LaTex');
yhand = ylabel('CDF of ASes');
set(a1, 'MarkerSize', markerSize);
set(gca,'XScale','log');
set(gca,'fontsize',boxRatioTextSize);
%set(gcf,'unit','centimeters','position',[10 5 7 5]);
set(xhand, 'FontSize', labelTestSize);
set(yhand, 'FontSize', labelTestSize);
set(gca,'xtick',[1 10 100 1000 10000 100000 1000000 10000000]);
set(gca,'XTickLabel',{'10^0','10^1','10^2', '10^3', '10^4', '10^5', '10^6','10^7'}); % Change x-axis ticks labels to desired values.
set(gca,'ytick',[0.2 0.4 0.6 0.8 1.0]);
% set(gca,'XTickLabel',{'1','10^2', '10^3', '10^4', '10^5', '10^6','10^7','10^8'});
xlim([1,10000000]);
grid on;

hold off;
