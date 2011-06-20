function [t y]=readoutput2(fname)
% read eegcode.output

if ~exist('fname','var')
    fname='../../neurofield.output';
end

fid=fopen(fname);

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
%outputdata=zeros(ntraces,2);
% % old format
% for i=1:ntraces,
%     outputdata(i,:)=sscanf(fgetl(fid),'Wave Equation Number : %d Node Number : %d');
% end
temp=fgetl(fid);
while length(temp)>20,
    temp=fgetl(fid);
end
y=fscanf(fid,'%f');
y=reshape([sscanf(temp,'%f');y],ntraces,nsteps/(skippoints+1))';
t=[0:deltat*(skippoints+1):(nsteps-1)*deltat]';

fclose(fid);
