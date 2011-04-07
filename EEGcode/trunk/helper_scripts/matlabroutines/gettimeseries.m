function dummy=gettimeseries(fname,k)
% get time series from fname eegcode output file in 2s epochs
% plotting traces number 'k'
% dummy=gettimeseries(fname,k)

fid=fopen(fname);
%
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
%
% Prepare to load data in epochs
%
epoch=2; % Set the epoch for the power spectrum at 2 sec
numspectra=fix((nsteps*deltat)/epoch);
numptsinspectra=fix(epoch/deltat);
n2read=numptsinspectra*ntraces;
%
y=fscanf(fid,'%f',n2read-1); % read in the lead in plus the first epoch
y=[sscanf(temp,'%f');y]; % rebuild the first line of y which was read earlier
%
% Reshape the array to the proper dimensions (time, trace number)
%
y=reshape(y,ntraces,numptsinspectra)';
%
% Store time series
%
tmseries=y(:,k);
%
n2read=numptsinspectra*ntraces;
for i=1:numspectra-1;
  y=fscanf(fid,'%f',n2read); % read in the next epoch
  y=reshape(y,ntraces,numptsinspectra)';
  tmseries=[tmseries',y(:,k)']'; % store time series
end
t=[0:deltat:(numptsinspectra*numspectra-1)*deltat]';
plot(t,tmseries)
dummy=0;

fclose(fid);
end

