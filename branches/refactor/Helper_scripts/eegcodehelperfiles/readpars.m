function pars=readpars(fname)
% function pars=readpars(fname)
%  read EIRS model parameters from eegcode.conf file
%  modified from getlinw.m
%  assumes all qmax, sigma, theta equal, and only gammae is finite

if ~exist('fname','var')
    fname='eegcode.conf';
end
fid=fopen(fname);
if(fid<0)
  error('Unable to open file')
end

deltat=findval(fid,'Deltat');
theta=findval(fid,'Theta');
sigma=findval(fid,'Sigma');
qmax=findval(fid,'Qmax');
phin_ic=findval(fid,'Mean');
gammae=findval(fid,'gamma');
deltax=findval(fid,'Deltax');
re=findval(fid,'Effective range');

fclose(fid);
fopen(fname);

alpha_ee=findval(fid,'alpha');
alpha_ei=findval(fid,'alpha');
alpha_es=findval(fid,'alpha');
temp=findval(fid,'alpha'); % skip one alpha value (ie)
temp=findval(fid,'alpha'); % skip one alpha value (ii)
temp=findval(fid,'alpha'); % skip one alpha value (is)
alpha_re=findval(fid,'alpha');
alpha_rs=findval(fid,'alpha');
alpha_se=findval(fid,'alpha');
alpha_sr=findval(fid,'alpha');
alpha_sn=findval(fid,'alpha');

fclose(fid);
fopen(fname);

beta_ee=findval(fid,'beta');
beta_ei=findval(fid,'beta');
beta_es=findval(fid,'beta');
temp=findval(fid,'beta'); % skip one beta value (ie)
temp=findval(fid,'beta'); % skip one beta value (ii)
temp=findval(fid,'beta'); % skip one beta value (is)
beta_re=findval(fid,'beta');
beta_rs=findval(fid,'beta');
beta_se=findval(fid,'beta');
beta_sr=findval(fid,'beta');
beta_sn=findval(fid,'beta');

fclose(fid);
fopen(fname);

phie_ic=findval(fid,'Initial Q');
phii_ic=findval(fid,'Initial Q');
phir_ic=findval(fid,'Initial Q');
phis_ic=findval(fid,'Initial Q');
noiseamp=findval(fid,'Amplitude');

% Find first non-zero tauab (tause)
tause=findval(fid,'Tauab');
while (0==tause)
  tause=findval(fid,'Tauab');
end
% Find second non-zero tauab (taure)
taure=findval(fid,'Tauab');
while (0==taure)
  taure=findval(fid,'Tauab');
end
% Find third non-zero tauab (taues)
taues=findval(fid,'Tauab');
while (0==taues)
  taues=findval(fid,'Tauab');
end
% Rescale by deltat and calc t0
taues=taues*deltat;
tause=tause*deltat;
t0=taues+tause;

nuee=findval(fid,'Nu');
temp=findval(fid,'Nu'); % skip one Nu value (ie)
nure=findval(fid,'Nu');
nuse=findval(fid,'Nu');
nuei=findval(fid,'Nu');
temp=findval(fid,'Nu'); % skip one Nu value (ii)
nusr=findval(fid,'Nu');
nues=findval(fid,'Nu');
temp=findval(fid,'Nu'); % skip one Nu value (is)
nurs=findval(fid,'Nu');
nusn=findval(fid,'Nu');

%phie=findroot(nuee, nuei, nues, nuse, nure, nusr, nurs, nusn,phin)
%phir=sigmoid( nure*phie + (nurs/nues)*(isigmoid(phie) - (nuee+nuei)*phie) );
%phis=sigmoid( nuse*phie + nusn*phin + nusr*phir );
%[Gee, Gei, Ges, Gse, Gre, Gsr, Grs, Gsn]=calcgains(nuee, nuei, nues, nuse, nure, nusr, nurs, nusn, sigma, qmax, phie, phir, phis);

alphas=[alpha_ee,alpha_ei,alpha_es,alpha_se,alpha_sr,alpha_sn,alpha_re,alpha_rs];
betas=[beta_ee,beta_ei,beta_es,beta_se,beta_sr,beta_sn,beta_re,beta_rs];
nus=[nuee,nuei,nues,nuse,nusr,nusn,nure,nurs];

pars.qmax=qmax;
pars.sigma=sigma;
pars.theta=theta;
pars.taues=taues;
pars.tause=tause;
pars.t0=t0;
pars.gammae=gammae;
pars.alphas=alphas;
pars.alpha_ee=alpha_ee;
pars.alpha_ei=alpha_ei;
pars.alpha_es=alpha_es;
pars.alpha_se=alpha_se;
pars.alpha_sr=alpha_sr;
pars.alpha_sn=alpha_sn;
pars.alpha_re=alpha_re;
pars.alpha_rs=alpha_rs;
pars.betas=betas;
pars.beta_ee=beta_ee;
pars.beta_ei=beta_ei;
pars.beta_es=beta_es;
pars.beta_se=beta_se;
pars.beta_sr=beta_sr;
pars.beta_sn=beta_sn;
pars.beta_re=beta_re;
pars.beta_rs=beta_rs;
pars.nus=nus;
pars.nuee=nuee;
pars.nuei=nuei;
pars.nues=nues;
pars.nuse=nuse;
pars.nusr=nusr;
pars.nusn=nusn;
pars.nure=nure;
pars.nurs=nurs;
pars.phie_ic=phie_ic;
pars.phii_ic=phii_ic;
pars.phis_ic=phis_ic;
pars.phir_ic=phir_ic;
pars.phin_ic=phin_ic;
pars.noiseamp=noiseamp;
pars.deltax=deltax;
pars.re=re;

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

end



