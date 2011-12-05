% pas.m: plots the voltage response and stdp curve induced by paired
% associative stimulation (PAS)

function pas(isi, directory)

if ~exist('directory','var')
    directory = '../../Output/';
end

%% plot voltage response and glutamate binding for different ISIs
figure;
for ii = 1:length(isi)
dir = [ directory, 'Stimulus_mode:_10,ISI=', num2str( isi(ii) ), '/' ];

fid=fopen([dir,'neurofield.output']);
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
[t ~] = readoutput2([dir,'neurofield.output']);

for jj = 1:length(couplings)
    subplot( 6, 4, [ii ii+4] );
    xlabel('Time (s)'); title(['ISI = ',num2str(isi(ii)*1e3),' ms']);
    color = [jj/length(couplings),0,1-jj/length(couplings)];
    y = textread( [dir,'neurofield.vout.', num2str(couplings(jj))] );
    hold on; box on; xlim([.9 1.3]);
	plot(t,y.*1e3,'--','Color',color)
	y = textread( [dir,'neurofield.bindout.', num2str(couplings(jj))] );
	plot(t,y.*10,':','Color',color)
    line([t(1) t(end)],[2.5 2.5],'LineStyle','--','Color','cyan');
    line([t(1) t(end)],[4.6 4.6],'LineStyle','--','Color','cyan');
	y = textread( [dir,'neurofield.caout.', num2str(couplings(jj))] );
	plot(t,y.*1e7,'Color',color)
end

end
%% plot STDP window

[~,stdp] = system(['./stdp.pl ',directory,' ',num2str(100000),' | sort -n']);
stdp = str2num(stdp);
subplot( 6,4,9:24); plot( stdp(:,1).*1e3, stdp(:,2), 'o-' );
line([stdp(1,1) stdp(end,1)].*1e3,[0 0],'LineStyle','--','Color','cyan');
xlim([stdp(1,1) stdp(end,1)].*1e3);
xlabel('ISI (ms)'); ylabel('% change in synaptic strength');
return; % to print output with config file, evaluate the following lines
%%
saveas(gcf,'.pic.pdf');
system(['plot_synaptout.sh ../../Output/Stimulus_mode:_10,ISI=0/neurofield.conf .pic.pdf'])