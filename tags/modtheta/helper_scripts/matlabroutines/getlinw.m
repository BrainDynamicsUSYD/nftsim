function dummy=getlinw(fname)
%function dummy=getlinw(fname)
%read EIRS model parameters from eegcode.conf file
%and plot frequency power spectrum
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
temp=findval(fid,'Nu'); % skip one Nu value
nure=findval(fid,'Nu');
nuse=findval(fid,'Nu');
nuei=findval(fid,'Nu');
temp=findval(fid,'Nu'); % skip one Nu value
nusr=findval(fid,'Nu');
nues=findval(fid,'Nu');
temp=findval(fid,'Nu'); % skip one Nu value
nurs=findval(fid,'Nu');
nusn=findval(fid,'Nu');

phie=findroot(nuee, nuei, nues, nuse, nure, nusr, nurs, nusn,phin)
phir=sigmoid( nure*phie + (nurs/nues)*(isigmoid(phie) - (nuee+nuei)*phie) );
phis=sigmoid( nuse*phie + nusn*phin + nusr*phir );
[Gee, Gei, Ges, Gse, Gre, Gsr, Grs, Gsn]=calcgains(nuee, nuei, nues, nuse, nure, nusr, nurs, nusn, sigma, qmax, phie, phir, phis);
%
% Create the omega array
%
npts = 10000;
omega = zeros(npts,1);
for i=1:npts
  omega(i) = i*100.*2.*pi/(1.0*npts);
end
%
% Call phiephin
%
transferfn = phiephin(gamma, Gee, Gei, Ges, Gse, Gre, Gsr, Grs, Gsn, t0, alpha, beta, omega);

%
% Plot the transfer function
%
loglog(omega/(2*pi), abs(transferfn).^2)
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

function retval=getterm(x,nuee, nuei, nues, nuse, nure, nusr, nurs, nusn,phin)
% Calculate the term to be minimised to find root of steady state equation
phir=sigmoid( nure*x + (nurs/nues)*(isigmoid(x) - (nuee+nuei)*x) );
phis=sigmoid( nuse*x + nusn*phin + nusr*phir );
retval=sigmoid( (nuee+nuei)*x + nues*phis) - x;
return
end

function retval=sigmoid(x)
global sigma;
global qmax;
global theta;
retval=qmax/( 1.0+exp((theta-x)/sigma) );
return
end

function retval=isigmoid(x)
global sigma;
global qmax;
global theta;
retval=theta-sigma*log(qmax/x -1.0);
return
end

function retval=findroot(nuee, nuei, nues, nuse, nure, nusr, nurs, nusn,phin)
deltax=1.0;
comp=getterm(0.5,nuee, nuei, nues, nuse, nure, nusr, nurs, nusn,phin);
for i=1:1:250
  comp1=getterm(i*deltax, nuee, nuei, nues, nuse, nure, nusr, nurs, nusn,phin);
  if (comp/comp1<0)
    lower=(i-1)*deltax;
    upper=i*deltax;
    break;
  end
end
prec=0.0001; % precision requirement for bisection method
comp=getterm(lower,nuee, nuei, nues, nuse, nure, nusr, nurs, nusn,phin);
while(upper-lower >prec)
  midpoint=(upper+lower)/2.0;
  comp1=getterm(midpoint,nuee, nuei, nues, nuse, nure, nusr, nurs, nusn,phin);
  if(comp/comp1<0)
    upper=midpoint;
  else
    lower=midpoint;
    comp=comp1;
  end
end
retval=(lower+upper)/2.0;
return
end
