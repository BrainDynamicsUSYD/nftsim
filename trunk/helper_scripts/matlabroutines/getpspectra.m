function dummy=getpspectra(fname,k,leadin)
% function dummy=getpspectra(fname,k,leadin)
% Get powerspectra from fname eegcode output file in 2s epochs
% at node index k. Drop the first leadin*epoch
% time as transient

fid=fopen(fname);

%
inamp=0.1; % This should be equal to the input power in stimulus i.e. Amplitude
          % in eegcode.conf
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
inputpower=(2*inamp*inamp/deltat)/(skippts+1); % Formula for coherent white noise input power
                                               % i.e. stimulus mode 4
                                               % (skippts+1) is alias scale
                                               % factor for white noise.
                                               % Output signal does not get
                                               % aliased.
temp=fgetl(fid);
while length(temp)>20,
    temp=fgetl(fid);
end
%
% Prepare to load data in epochs
%
epoch=2; % Set the epoch for the power spectrum at 2 sec
numspectra=fix((nsteps*deltat)/epoch)-ceil(leadin);
numptsinspectra=fix(epoch/deltat);
n2read=numptsinspectra*ntraces*(1+leadin);
%
y=fscanf(fid,'%f',n2read-1); % read in the lead in plus the first epoch
y=[sscanf(temp,'%f');y]; % rebuild the first line of y which was read earlier
%
% strip out the lead in transient
%
bgn=n2read*(leadin/(leadin+1))+1;
fnsh=n2read;
y=y(bgn:fnsh,1);
%
% Reshape the array to the proper dimensions (time, trace number)
%
y=reshape(y,ntraces,numptsinspectra)';
t=[0:deltat:(nsteps-1)*deltat]';

%plot(t,y)
%
% Calculate the power spectra
%
freq=[0:1.0/((numptsinspectra-1)*deltat):1/(deltat)].';
powerspectra=squeeze(zeros(numptsinspectra,1));
%
% k is index for the powerspectra to be calculated
%
%
removezero=mean(y(:,k));
y=y-removezero;
array = abs(fft(y(:,k))).^2;
powerspectra=powerspectra+array;
n2read=numptsinspectra*ntraces;
for i=1:numspectra-1;
  y=fscanf(fid,'%f',n2read);
  y=reshape(y,ntraces,numptsinspectra)';
  removezero=mean(y(:,k));
  y=y-removezero;
  array = abs(fft(y(:,k))).^2;
  powerspectra=powerspectra+array;
end
powerspectra=powerspectra/numspectra;
loglog(freq,powerspectra/inputpower)
lowerbound=min(find(freq > 100));
axis([1,100,powerspectra(lowerbound),max(powerspectra)])
hold on
axis manual
dummy=0;

fclose(fid);
end

