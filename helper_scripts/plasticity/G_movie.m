% G_movie.m: creates a movie over the spatial variation of gains

dir = './';
nodelength = 50;

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
% couplings = [3,5];

for coupling = 1:length(couplings)
    fid = fopen( ['../../',dir,'neurofield.synaptout.',num2str(couplings(coupling))] );
    avi = avifile(['G_movie.',num2str(couplings(coupling)),'.avi']);
    h = figure; hm = surfc(0:nodelength-1,0:nodelength-1,zeros(nodelength,nodelength), ...
        'EdgeColor','none','LineStyle','none','FaceLighting','phong');
    box on; colorbar; colormap('hot');
    set(gca,'xtick',[]); set(gca,'ytick',[]);
    xlabel('x'); ylabel('y'); ht = title(['G(',num2str(couplings(coupling)),')']);
    view(2);
    G = zeros(nodelength,nodelength);
    for t = 1:nsteps
        for node = 1:nodelength^2
            G(node) = sscanf(fgetl(fid),'%f');
        end
        Gmean = mean(mean(G));
        set( hm, 'ZData', G-Gmean )
        set(ht,'String',['G(',num2str(couplings(coupling)),') = ',num2str(Gmean),...
            ', fluctuation = ',num2str(max(max(abs(G-Gmean)))),', t = ', num2str(t*deltat), ' s']);
        set(gca,'Clim',[-4e-0 +4e-0]);
        drawnow
        F = getframe(h);
        avi = addframe(avi,F);
    end
    avi = close(avi);
    fclose(fid);
end