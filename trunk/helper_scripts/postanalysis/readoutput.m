% read eegcode.output

fid=fopen('eegcode.output');

deltat=sscanf(fgetl(fid),'Deltat :%g');
nsteps=sscanf(fgetl(fid),'Number of integration steps :%d');
nsteps
ntraces=sscanf(fgetl(fid),'Output Data - Number of traces: %d');
outputdata=zeros(ntraces,2);
% % old format
% for i=1:ntraces,
%     outputdata(i,:)=sscanf(fgetl(fid),'Wave Equation Number : %d Node Number : %d');
% end
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
epoch=2 % Set the epoch for the power spectrum at 2 sec
numspectra=int64((nsteps*deltat)/epoch)
numpointsinspectra=int64(epoch/deltat)
freq=[0:1.0/(nsteps*deltat):1/(deltat)]
%
for k=0:(ntraces-1)
  for i=0:(numspectra-1)
    removezero=mean(y[k,((i+0.5)*numptsinspectra):((i+1.5)*numptsinspectra-1)])
    y[k,((LONG(i)+0.5)*numptsinspectra):((LONG(i+1)+0.5)*numptsinspectra-1)]-=removezero
    array = ABS(FFT(y[k,((LONG(i)+0.5)*numptsinspectra):((LONG(i+1)+0.5)*numptsinspectra-1)]))^2
    powerspectra=powerspectra+array
  endfor
  powerspectra=powerspectra/numspectra
end
plot (freq,powerspectra)
