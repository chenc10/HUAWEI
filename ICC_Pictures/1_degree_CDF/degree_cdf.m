

lineWidth = 1;
markerSize=2;
labelTestSize = 10;
legendTextSize = 25;
boxRatioTextSize = 10;

x=[1, 2, 3, 4, 6, 13, 66, 340, 1715, 8741];
y=[0.15972637055593025, 0.41414394450332404, 0.5812891415357935, 0.6821273725792466, 0.7845071779554871, 0.8911263127468928, 0.9709605935061181, 0.9948164563060025, 0.9994797186626843, 1.0]

figure(1);
hold on;


a1 = plot(x, y,'o-k','LineWidth',lineWidth,'color','b');
%line([5.76,5.76],[0,1])
xhand = xlabel('degree');
yhand = ylabel('CDF of AS');
set(a1, 'MarkerSize', markerSize);
set(gca,'XScale','log');
set(gca,'fontsize',boxRatioTextSize)
set(xhand, 'FontSize', labelTestSize);
set(yhand, 'FontSize', labelTestSize);
% set(gcf,'unit','centimeters','position',[10 5 7 5]);

set(gca,'xtick',[1 10 100 1000 10000]);
set(gca,'XTickLabel',{'10^0','10^1','10^2', '10^3', '10^4', '10^5'}); % Change x-axis ticks labels to desired values.
set(gca,'ytick',[0.2 0.4 0.6 0.8 1.0]);
% set(gca,'XTickLabel',{'1','10^2', '10^3', '10^4', '10^5', '10^6','10^7','10^8'});
xlim([1,10000]);
grid on;
hold off;
