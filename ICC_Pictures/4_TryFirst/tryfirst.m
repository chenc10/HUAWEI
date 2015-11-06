lineWidth = 1;
markerSize=2;
labelTestSize = 10;
legendTextSize = 25;
boxRatioTextSize = 10;


% x=[2	3	10	162 1253    12559	729021	7974986	11416247	23124464];
% y=[6.749	6.749	6.746   6.74   6.66	6.617	6.587	6.553	6.287	5.837]
x=[1	2	3	6	10	40	393	3370	21565	51895]
y=[5.837	6.038   6.287	6.483	6.584	6.587	6.601	6.617	6.746	6.749]
y=y-1
figure(1);
hold on;


a1 = stem(x, y,'o-k','LineWidth',lineWidth,'color','b');
a2 = plot(x, y,'--','LineWidth',lineWidth,'color','r');
a3=line([1,51895],[5.76,5.76]);
xhand = xlabel('serial number of AS ordered by neighborhood weight', 'Interpreter','tex');
%xlabel('\textit{neighborhood weight}', 'Interpreter','LaTex');
yhand = ylabel('average global convergence delay');
set(a1, 'MarkerSize', markerSize);
set(gca, 'XScale','log');
set(gca,'fontsize',boxRatioTextSize)
set(xhand, 'FontSize', labelTestSize);
set(yhand, 'FontSize', labelTestSize);
ylim([4,6]);
grid on;
hold off;
