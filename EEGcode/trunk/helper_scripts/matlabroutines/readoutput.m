% read eegcode.output

fid=fopen('eegcode.output');

deltat=sscanf(fgetl(fid),'Deltat :%g');
nsteps=sscanf(fgetl(fid),'Number of integration steps :%d');
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