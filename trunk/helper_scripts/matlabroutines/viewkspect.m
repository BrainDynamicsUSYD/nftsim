load kwspect.mat
%
% plot a omega, |k| surface
%
fid=fopen('eegcode.output');
tline=fgetl(fid);
if(tline(1)=='S')
    temp=sscanf(tline,'Skippoints :%i Deltat :%g');
    deltat=temp(2)*(temp(1)+1);
    skippts=temp(1);
    nsteps=sscanf(fgetl(fid),'Number of integration steps :%d')/(skippts+1);
else
    deltat=sscanf(tline,'Deltat :%g');
    skippts=0;
    nsteps=sscanf(fgetl(fid),'Number of integration steps :%d');
end
ntraces=sscanf(fgetl(fid),'Output Data - Number of traces: %d');
temp=fgetl(fid);
while length(temp)>20,
    temp=fgetl(fid);
end
epoch=2; % Set the epoch for the power spectrum at 2 sec
numspectra=fix((nsteps*deltat)/epoch)-ceil(leadin);
numptsinspectra=fix(epoch/deltat);
freq=[0:1.0/(numptsinspectra*deltat):1/(2*deltat)].';
deltax=0.0035;
[omegak,karray]=resortlattice(powerspectra,deltax,size(powerspectra,2));
contour(freq,karray,omegak)
%contour(freq,karray,omegak,logcontour(omegak,3))
print('kspect.eps')

