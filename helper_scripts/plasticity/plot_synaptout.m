% plot_synaptout.m
% reads output from neurofield.output and neurofield.synaptout.*
% and plots the firing rate and nu's

dir = './';

fid=fopen(['../../',dir,'neurofield.output']);
temp=fgetl(fid);
if temp(1)=='S',
    vals=sscanf(temp,'Skippoints: %d Deltat :%g');
    skippoints=vals(1);
    deltat=vals(2);
else
    deltat=sscanf(temp,'Deltat: %g');
    skippoints=0;
end
nsteps=sscanf(fgetl(fid),'Number of integration steps:%d');
ntraces=sscanf(fgetl(fid),'Output Data - Number of traces: %d');
temp = fgetl(fid);
couplings = sscanf(temp,'Propagator Number : %d');
while length(temp)>20
    temp = sscanf( fgetl(fid), 'Propagator Number : %d' );
    couplings = [ couplings, temp ];
end
fclose(fid);

% the above code assumes all outputed connections are plastic
% if that is not true, set the plastic connection indices manually here:
couplings = [2];

figure;

subplot(2,2,1); hold on; box on; xlabel('Time (s)'); ylabel('Firing rate (s^{-1})')
[t y] = readoutput2(['../../',dir,'neurofield.output']);
for loop = 1:length(couplings)
    % plot only for the one node, regardless of spatial variation
    plot(t,y(:,loop*ntraces/length(couplings)));
end
h = get(gca,'Children');
for loop = 1:length(h)
    set( h(loop), 'Color', [loop/length(h),0,1-loop/length(h)] );
end

subplot(2,2,2); box on; xlabel('Time (s)'); ylabel('V (mV)'); hold on
for loop = 1:length(couplings)
    color = [loop/length(couplings),0,1-loop/length(couplings)];
	y = textread( ['../../',dir,'neurofield.vout.', num2str(couplings(loop))] );
	plot(t,y.*1e3,'Color',color)
end

subplot(2,2,3); box on; xlabel('Time (s)'); ylabel('[Ca] (\muM)'); hold on
for loop = 1:length(couplings)
    color = [loop/length(couplings),0,1-loop/length(couplings)];
	y = textread( ['../../',dir,'neurofield.caout.', num2str(couplings(loop))] );
	plot(t,y.*1e6,'Color',color)
end

subplot(2,2,4); box on; xlabel('Time (s)'); ylabel('|G|'); hold on
for loop = 1:length(couplings)
    color = [loop/length(couplings),0,1-loop/length(couplings)];
	y = textread( ['../../',dir,'neurofield.synaptout.', num2str(couplings(loop))] );
	plot(t,abs(y),'Color',color)
end

y = 0;%-0.0018*4200;
for loop = 1:length(couplings)
	tempy = textread( ['../../',dir,'neurofield.synaptout.', num2str(couplings(loop))] );
    y = y + tempy;
end
plot(t,1.*abs(y),'k','LineWidth',2);

% subplot(3,2,5);
% for k = 1:length(couplings)
%     spectrum( ['../../',dir,'neurofield.output'] ,k); hold on
% end
% h = get(gca,'Children');
% for loop = 1:length(h)
%     set( h(loop), 'Color', [loop/k,0,1-loop/k] );
% end
% 
% subplot(3,2,6);
% for k = 1:length(couplings)
%     kspectrum( ['../../',dir,'neurofield.output'], k, 2); hold on
% end
% h = get(gca,'Children');
% for loop = 1:length(h)
%     set( h(loop), 'Color', [loop/k,0,1-loop/k] );
% end

return; % to print output with config file, evaluate the following lines
saveas(gcf,'.pic.pdf');
system(['plot_synaptout.sh ../../',dir,'neurofield.conf .pic.pdf'])