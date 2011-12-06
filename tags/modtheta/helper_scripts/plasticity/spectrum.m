function spectrum(fname,k)
% get powerspectra from fname

if ~exist('fname','var')
    fname = '../../neurofield.output.proper';
end
if ~exist('k','var')
    k = 1;
end
fid=fopen(fname);

tline=fgetl(fid);
if(tline(1)=='S')
    temp=sscanf(tline,'Skippoints :%i Deltat :%g');
    deltat=temp(2)*(temp(1)+1);
    skippts=temp(1);
    nsteps=sscanf(fgetl(fid),'Number of integration steps :%d')/(skippts+1);
else
    deltat=sscanf(tline,'Deltat :%g');
    nsteps=sscanf(fgetl(fid),'Number of integration steps :%d');
end
ntraces=sscanf(fgetl(fid),'Output Data - Number of traces: %d');
temp=fgetl(fid);
% while length(temp)>20,
%     temp=fgetl(fid); % gets rid of the lines 'Propagator Number: ...'
% end
% y=fscanf(fid,'%f');
% y=reshape([sscanf(temp,'%f');y],ntraces,nsteps)';
% fclose(fid);

% read the last 30 seconds of firing activity to generate spectrum

system(['tail -n',num2str(30/deltat),' ',fname, ...
    ' | grep -v "^$" > .spectrum.output']);
y = importdata('.spectrum.output');
x = y(k:ntraces:end);
!rm .spectrum.output

[powerspectra,f] = pwelch(x-mean(x),5/deltat,1/deltat,1/deltat,1/deltat);
loglog(f,powerspectra');
xlabel('f (Hz)'); ylabel('Power Spectrum');