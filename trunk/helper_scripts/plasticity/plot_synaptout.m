% plot_synaptout.m
% reads output from neurofield.output and neurofield.synaptout.*
% and plots the firing rate and nu's

dir = './';

figure;

[t y] = readoutput2(['../../',dir,'neurofield.output']);
subplot(3,2,1); box on;
plot(t,y)
xlabel('Time (s)'); ylabel('Firing rate')

couplings = [3,5]; % all coupleplast indices are listed here

subplot(3,2,2); box on; xlabel('Time (s)'); ylabel('V (V)'); hold on
for loop = 1:length(couplings)
    color = [loop/length(couplings),0,1-loop/length(couplings)];
	y = textread( ['../../',dir,'neurofield.vout.', num2str(couplings(loop))] );
	plot(t,y,'Color',color)
end

subplot(3,2,3); box on; xlabel('Time (s)'); ylabel('[Ca] (M)'); hold on
for loop = 1:length(couplings)
    color = [loop/length(couplings),0,1-loop/length(couplings)];
	y = textread( ['../../',dir,'neurofield.caout.', num2str(couplings(loop))] );
	plot(t,y,'Color',color)
end

subplot(3,2,4); box on; xlabel('Time (s)'); ylabel('G'); hold on
for loop = 1:length(couplings)
    color = [loop/length(couplings),0,1-loop/length(couplings)];
	y = textread( ['../../',dir,'neurofield.synaptout.', num2str(couplings(loop))] );
	plot(t,y,'Color',color)
end

y = 0;%-0.0018*4200;
for loop = 1:length(couplings)
	tempy = textread( ['../../',dir,'neurofield.synaptout.', num2str(couplings(loop))] );
    y = y + tempy;
end
plot(t,y,'k','LineWidth',2);

subplot(3,2,5);
for k = 1:2
    spectrum( ['../../',dir,'neurofield.output'] ,k); hold on
end
h = get(gca,'Children');
for loop = 1:length(h)
    set( h(loop), 'Color', [loop/k,0,1-loop/k] );
end

subplot(3,2,6); set(gca,'xtick',[]); set(gca,'ytick',[]);
text(0.1,0.1,{date, ['Output from ',dir]},'VerticalAlignment','bottom','FontSize',8 );

return; % to print output with config file, evaluate the following lines
saveas(gcf,'.pic.pdf');
system(['plot_synaptout.sh ../../',dir,'neurofield.conf .pic.pdf'])