function dummy=intrarsteadystate(fname)
%function dummy=intrarsteadystate(fname)
%read EIRS model parameters from eegcode.conf file
%and plot steady state contours when intrathalamic connection
%nu_rr is present
fid=fopen(fname);
if(fid<0)
  'Unable to open file'
  dummy=-1;
  return
end

global sigma;
sigma=findval(fid,'Sigma');
alpha=findval(fid,'alpha');
fclose(fid);
fopen(fname);
global theta;
theta=findval(fid,'Theta');
fclose(fid);
fopen(fname);
global qmax;
qmax=findval(fid,'Qmax');
beta=findval(fid,'beta');
gamma=findval(fid,'gamma');
fclose(fid);
fopen(fname);
deltat=findval(fid,'Deltat');
phin=findval(fid,'Mean');

% Find first non-zero tauab
tauab=findval(fid,'Tauab');
while (0==tauab)
  tauab=findval(fid,'Tauab');
end
% Find second non-zero tauab
tauab1=findval(fid,'Tauab');
while (0==tauab1)
  tauab1=findval(fid,'Tauab');
end
% Find third non-zero tauab
tauab1=findval(fid,'Tauab');
while (0==tauab1)
  tauab1=findval(fid,'Tauab');
end
% Combine first and third tauab
t0=(tauab+tauab1)*deltat;

nuee=findval(fid,'Nu');
temp=findval(fid,'Nu'); % skip one Nu value nuie
nure=findval(fid,'Nu');
nuse=findval(fid,'Nu');
nuei=findval(fid,'Nu');
temp=findval(fid,'Nu'); % skip one Nu value nuii
nurr=findval(fid,'Nu');
nusr=findval(fid,'Nu');
nues=findval(fid,'Nu');
temp=findval(fid,'Nu'); % skip one Nu value nuis
nurs=findval(fid,'Nu');
nusn=findval(fid,'Nu');
%
% Create phie array
%
npts = 250;
phie = zeros(npts,1);
for i=1:npts-1
  phie(i) = i*qmax/(1.0*npts);
end
phie = ones(npts,1) * phie';
%
% Create phir array
%
phir = zeros(npts,1);
for i=1:npts-1
  phir(i) = i*qmax/(1.0*npts);
end
phir = phir * ones(npts,1)';
% Create array to hold plotted equations
eq1 = zeros(npts,npts);
eq2 = zeros(npts,npts);

eq1=-isigmoid(phie)+(nuee+nuei)*phie+nues*sigmoid(nuse*phie+nusr*phir+nusn*phin);
eq2=-isigmoid(phir)+nure*phie+nurr*phir+nurs*sigmoid(nuse*phie+nusr*phir+nusn*phin);

phie=phie(1,1:npts-1);
phir=phir(1:npts-1,1);
vn=[0,0]; % Contour levels only a h=0
%Plot each graph
contour(phir,phie',eq1(1:npts-1,1:npts-1),vn,"2");
hold on
contour(phir,phie',eq2(1:npts-1,1:npts-1),vn,"3");
hold off
dummy=0;
return
end

function retval=findval(fid,paramstr)
%find a parameter in conf file
i=1;
tline=fgetl(fid);
while 0==feof(fid),
  pos=strfind(tline,paramstr);
  if (pos>0)
    [temp,last]=strtok(tline,':');
    while (strfind(last,paramstr)>0)
      [temp,last]=strtok(last,':');
    end
    retval=sscanf(last,':%f');
    return
  end
  tline=fgetl(fid);
  i=i+1;
end
retval=NaN;
return
end

function retval=sigmoid(x)
global sigma;
global qmax;
global theta;
retval=qmax./( 1.0.+exp((theta.-x)./sigma) );
return
end

function retval=isigmoid(x)
global sigma;
global qmax;
global theta;
retval=theta.-sigma.*log(qmax./x .-1.0);
return
end
