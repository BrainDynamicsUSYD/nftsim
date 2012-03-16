function dummy=plotkspect(fname,k,leadin,xlen,ylen,deltax,deltay)
% function dummy=plotkspect(fname,k,leadin,xlen,ylen,deltax,deltay)
% get kw spectra from fname in 2s epochs
% writing the result in the kwspect.mat file in the
% matlab v7 binary form as powerspectra.
%
inamp=0.1; % This should be equal to the input power in stimulus i.e. Amplitude
           % in eegcode.conf
%
fid=fopen(fname);
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
epoch=2; % Set the epochs for the power spectrum at 2 sec
numspectra=fix((nsteps*deltat)/epoch)-ceil(leadin);
numptsinspectra=fix(epoch/deltat);
n2read=numptsinspectra*ntraces*(1+leadin);
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
%
% Further reshape so traces are in 2D array form
%
y=reshape(y,numptsinspectra,xlen,ylen);
%plot(t,y)
%
% Calculate the power spectra
%
freq=[0:1.0/(numptsinspectra*deltat):1/(2*deltat)].';
freqx=[0:1.0/(xlen*deltax):1/(2*deltax)].';
freqy=[0:1.0/(ylen*deltay):1/(2*deltay)].';
powerspectra=squeeze(zeros(numptsinspectra,xlen,ylen));
array = abs(fftn(y(:,:,:))).^2;
powerspectra=powerspectra+array;
n2read=numptsinspectra*ntraces;
for i=1:numspectra-1;
  y=fscanf(fid,'%f',n2read); % read in the next epoch
  y=reshape(y,ntraces,numptsinspectra)';
  y=reshape(y,numptsinspectra,xlen,ylen); % reshape the data to something useful
  array = abs(fftn(y(:,:,:))).^2;
  powerspectra=powerspectra+array;
end
powerspectra=powerspectra/numspectra;
powerspectra=powerspectra(1:fix(numptsinspectra/2)+1,1:fix(xlen/2)+1,1:fix(ylen/2)+1);
powerspectra(1,1,1)=0.;
%save('-mat7-binary','kwspect.mat','powerspectra')
save('kwspect.mat','powerspectra')
%
% plot a omega, |k| surface
%
%[omegak,karray]=resortlattice(powerspectra,deltax,size(powerspectra,2));
%contour(freq,karray,omegak,logcontour(omegak,3));
%print("kwspect.eps")
%figure(2)
%loglog(freq,omegak(1,:))
%mesh(freq,karray,omegak)
%
%mesh(freqx,freqx,squeeze(powerspectra(1,:,:))/inputpower)
%  set(get(h,'Parent'),'XScale','log')
%  set(get(h,'Parent'),'YScale','log')
%  set(get(h,'Parent'),'ZScale','log');
%hold on
%figure(3)
%contour(freqx,freqy,squeeze(powerspectra(1,:,:))/inputpower,40)
% axis manual
dummy=0;
fclose(fid);
end

