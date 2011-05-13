% plot_synaptout.m
% reads output from neurofield.output and neurofield.synaptout.*
% and plots the firing rate and nu's

dir = './';

[t y] = readoutput2(['../../',dir,'neurofield.output']);
figure
plot(t,y)
xlabel('Time (s)'); ylabel('Firing rate')

couplings = [3,5]; % all coupleplast indices are listed here

figure; xlabel('Time (s)'); ylabel('G')
hold on
for loop = 1:length(couplings)
    color = [loop/length(couplings),0,1-loop/length(couplings)];
	y = textread( ['../../',dir,'neurofield.synaptout.', num2str(couplings(loop))] );
	plot(t,y,'Color',color)
end

% figure; xlabel('Time (s)'); ylabel('[Ca] (M)')
% hold on
% for loop = 1:length(couplings)
%     color = [loop/length(couplings),0,1-loop/length(couplings)];
% 	y = textread( ['../../neurofield.caout.', num2str(couplings(loop))] );
% 	plot(t,y,'Color',color)
% end

system(['./G_total.pl ../../',dir, ' ', ...
    num2str(couplings(1)),' ',num2str(couplings(2))]);
y = textread( ['../../',dir,'neurofield.synaptout.total'] );
figure; plot(t,y); xlabel('Time (s)'); ylabel('G_e + G_i')

for k = 1:2
    spectrum( ['../../',dir,'neurofield.output'] ,k);
end