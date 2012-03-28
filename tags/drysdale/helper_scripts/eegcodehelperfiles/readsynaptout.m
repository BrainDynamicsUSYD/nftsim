function nu = readsynaptout(n)
% read eegcode.synaptout.n

fid=fopen(strcat('eegcode.synaptout.',num2str(n)));

%outputdata=zeros(ntraces,2);
% % old format
% for i=1:ntraces,
%     outputdata(i,:)=sscanf(fgetl(fid),'Wave Equation Number : %d Node Number : %d');
% end
temp=fgetl(fid);
ntraces=sscanf(temp,'Output Synapse - Number of traces: %d');
while length(temp)>20,
    temp=fgetl(fid);
end
nu=fscanf(fid,'%f');
nu=reshape([sscanf(temp,'%f');nu],ntraces,length(nu)+1)';
%t=[0:deltat:(nsteps-1)*deltat]';

fclose(fid);