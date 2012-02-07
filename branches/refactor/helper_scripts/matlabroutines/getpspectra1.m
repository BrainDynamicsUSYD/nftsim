function dummy=getpspectra(fname,k)
% get powerspectra from fname using the k th trace

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
y=fscanf(fid,'%f');
y=reshape([sscanf(temp,'%f');y],ntraces,nsteps)';
t=[0:deltat:(nsteps-1)*deltat]';

fclose(fid);
%plot(t,y)
%
% Calculate the power spectra
%
epoch=2; % Set the epoch for the power spectrum at 2 sec
numspectra=fix((nsteps*deltat)/epoch)-1;
numptsinspectra=fix(epoch/deltat);
freq=[0:1.0/((numptsinspectra-1)*deltat):1/(deltat)].';
powerspectra=squeeze(zeros(numptsinspectra,1));
size(powerspectra);
%
% k is index for the powerspectra to be calculated
%
%
  for i=0:numspectra-1;
    bgn=fix((i+0.5)*numptsinspectra);
    fnsh=fix((i+1.5)*numptsinspectra-1);
    removezero=mean(y(bgn:fnsh,k));
    y(bgn:fnsh,k)=y(bgn:fnsh,k)-removezero;
    array = abs(fft(y(bgn:fnsh,k))).^2;
    powerspectra=powerspectra+array;
  end
powerspectra=powerspectra/numspectra;
loglog(freq,powerspectra/inputpower)
lowerbound=min(find(freq > 100));
axis([1,100,powerspectra(lowerbound),max(powerspectra)])
hold on
axis manual
dummy=0;

end

