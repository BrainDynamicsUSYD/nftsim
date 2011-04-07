function ssepconf4(varargin)
% write eegcode.conf for SSEPs, with optional filename
% usual model plus an extra drive

%%%%%%%%%%%%%%%%%%%
% TODO: - TEST
%       - more sophisticated root finding to guard against bad ICs
%       - square and sine waves have different means, problem? Can the
%       drive be negative? i.e., can it directly inhibit the relay nuclei,
%       without having to go via the reticular nucleus or interneurons
%       - clean up varagin

% 
% ssepconf, !eeg
% readoutput, figure(6), plot(t,y), yy=y(end-pow2(nextpow2(end/2))+1:end,1);
% figure(1), psd(yy,32768,1/deltat), xlim([0 100])
% [P,f]=pspec(y(:,1),deltat,10,5); figure(2), plot(f,log(P)), xlim([0 100])

global par    % for storing params in to pass elsewhere more easily, numbered as in matlab code

confname='eegcode.conf';

% for input of stimulus type:
% case 1: //  Pulse stimulus pattern
%     Stimulus mode: 1 Time to start of stimulus: 0.0002 Amplitude: 20 Pulse Duration: 20 Pulse repetition period: 20   % OR: '...Pulse Duration: %d Pulse repetition period: %d\n',floor(2*nsteps*deltat),floor(2*nsteps*deltat)
% case 2: //  White noise stimulus pattern
%     Stimulus mode: 2 Time to start of stimulus: 0.0002 Amplitude: 20 Mean: 5
% case 3: //  Sinusoidal stimulus pattern
%     Stimulus mode: 3 Time to start of stimulus: 0.0002 Amplitude: 20 Modulation Frequency: 10
% case 4: //  Coherent white noise stimulus pattern (spatially coherent)
%     Stimulus mode: 4 Time to start of stimulus: 0.0002 Amplitude: 20 Mean: 5

% NOTE: square wave (case 1) is from phin=0 to phin=amplitude
%       sine wave (case 3) is from phin=-amp to phin=amp


whichstim=3;  % will use as above, tho won't use white noise for SSEP

stimstart=0; % in s
stimamp=0;
stimfreq=30; % in Hz

noiseamp=13;%0.1*100;
phin_ic=1;  % mean level of phin  % was 18

% params and ICs from parset

% integration data
nsteps=600000;%40*60*1000;
deltat=0.0001;

if nargin>0,
    stimparams=varargin{1};
    stimfreq=stimparams(1);
    if length(stimparams)>1,
        stimamp=stimparams(2);
    end
    if nargin>1,
        intparams=varargin{2};
        nsteps=intparams(1);
        deltat=intparams(2);
    end
    if nargin>2,
        confname=varargin{3};
    end
end

%%% CHECK ALHPAsr BETAsr

parset=2;  % 1==parsetect0asym, 2==parsetpmt0asym, 3==eegcode defaults, 4==HBM 04

if parset==1,  % from parsetect0asym
    a=0.5; par(1)=a;                                 %a=0.5  % 0<a<1, proportion thalamus->cortex
    theta=0.015; par(4)=theta;                       %theta=0.015
    sig0=0.006; par(7)=sig0; sigma=sig0*sqrt(3)/pi;  %sig0=0.006  % alt could define sigma, but matlab code uses sig0
    qmax=250; par(8)=qmax;                           %qmax=250
    re=0.08; par(10)=re;                             %re=0.08
    gammae=100; par(12)=gammae;                      %gammae=100
    alpha=80; par(13)=alpha;                         %alpha=60
    beta=10*alpha; par(14)=beta;                      %beta=4*alpha
    t0=0.08; par(15)=t0;                             %t0=0.08
    nuee=13e-4; par(20)=nuee;                        %nuee=12e-4
    nuei=-29e-4; par(21)=nuei;                       %nuei=-18e-4
    nues=1.3e-4; par(22)=nues;                        %nues=14e-4
    nuse=29.0e-4; par(23)=nuse;                        %nuse=10e-4
    nusr=-5.7e-4; par(24)=nusr;                       %nusr=-10e-4
    nusn=10e-4; par(25)=nusn;                        %nusn=10e-4  % NOT nusn*phin 
    nure=6.7e-4; par(26)=nure;                         %nure=2e-4
    nurs=29e-4; par(27)=nurs;                         %nurs=2e-4
    deltax=0.0035; par(28)=deltax;                   %deltax=0.0035
end

if parset==2,  % from parsetpmt0asym
    a=0.5; par(1)=a;                                 %a=0.5  % 0<a<1, proportion thalamus->cortex
    theta=0.015; par(4)=theta;                       %theta=0.015
    sig0=0.006; par(7)=sig0; sigma=sig0*sqrt(3)/pi;  %sig0=0.006  % alt could define sigma, but matlab code uses sig0
    qmax=250; par(8)=qmax;                           %qmax=250
    re=0.08; par(10)=re;                             %re=0.08
    gammae=100; par(12)=gammae;                      %gammae=100
    alpha=60; par(13)=alpha;                         %alpha=60
    beta=4*alpha; par(14)=beta;                      %beta=4*alpha
    t0=0.08; par(15)=t0;                             %t0=0.08
    nuee=12e-4; par(20)=nuee;                        %nuee=10e-4
    nuei=-18e-4; par(21)=nuei;                       %nuei=-18e-4
    nues=14e-4; par(22)=nues;                        %nues=32e-4
    nuse=10e-4; par(23)=nuse;                        %nuse=44e-4
    nusr=-10e-4; par(24)=nusr;                        %nusr=-8e-4
    nusn=10e-4; par(25)=nusn;                        %nusn=20e-4  % ==nusn*phin
    nure=2e-4; par(26)=nure;                        %nure=16e-4
    nurs=2e-4; par(27)=nurs;                         %nurs=6e-4
    deltax=0.0035; par(28)=deltax;                   %deltax=0.0035
end

if parset==3,  % from defaults above
    a=0.5; par(1)=a;                                 %a=0.5  % 0<a<1, proportion thalamus->cortex
    theta=0.013; par(4)=theta;                       %theta=0.013
    sigma=0.0033; sig0=sigma*pi/sqrt(3); par(7)=sig0;%sigma=0.0038
    qmax=250; par(8)=qmax;                           %qmax=250
    re=0.08; par(10)=re;                             %re=0.08
    gammae=100; par(12)=gammae;                      %gammae=100
    alpha=75; par(13)=alpha;                         %alpha=75
    beta=285; par(14)=beta;                          %beta=285
    t0=0.084; par(15)=t0;                            %t0=0.084
    nuee=11.2e-4; par(20)=nuee;                        %nuee=12e-4
    nuei=-18.84e-4; par(21)=nuei;                       %nuei=-18e-4
    nues=12e-4; par(22)=nues;                        %nues=12e-4
    nuse=12.74e-4; par(23)=nuse;                        %nuse=12e-4
    nusr=-8e-4; par(24)=nusr;                        %nusr=-8e-4
    nusn=1.5e-4; par(25)=nusn;                        %nusn=0.001  orig: nusn=0  % really phin==0 ?
    nure=2e-4; par(26)=nure;                         %nure=2e-4
    nurs=2e-4; par(27)=nurs;                         %nurs=2e-4
    deltax=0.0035; par(28)=deltax;                   %deltax=0.0035
end

if parset==4,  % eyes open, from HBM paper
    a=0.5; par(1)=a;                                 %a=0.5  % 0<a<1, proportion thalamus->cortex
    theta=0.013; par(4)=theta;                       %theta=0.013
    sigma=0.0038; sig0=sigma*pi/sqrt(3); par(7)=sig0;%sigma=0.0038
    qmax=340; par(8)=qmax;                           %qmax=340
    re=0.086; par(10)=re;                             %re=0.087
    gammae=116; par(12)=gammae;                      %gammae=116
    alpha=80; par(13)=alpha;                         %alpha=80 or, 1/alpha=12 ms
    beta=800; par(14)=beta;                          %beta=800 or, 1/beta=1.3 ms
    t0=0.085; par(15)=t0;                             %t0=0.085
    nuee=16e-4; par(20)=nuee;                        %nuee=16e-4
    nuei=-19e-4; par(21)=nuei;                       %nuei=-19e-4
    nues=3.9e-4; par(22)=nues;                        %nues=3.9e-4
    nuse=6.0e-4; par(23)=nuse;                        %nuse=6.0e-4
    nusr=-4.5e-4; par(24)=nusr;                       %nusr=-4.5e-4
    nusn=20*1.5e-4; par(25)=nusn;                        %nusn=1.5e-4  % ==nusn*phin
    nure=1.5e-4; par(26)=nure;                         %nure=1.5e-4
    nurs=0.3e-4; par(27)=nurs;                         %nurs=0.3e-4
    deltax=0.0035; par(28)=deltax;                   %deltax=0.0035
end


% alpha_ab
alpha_ee=alpha;
alpha_ei=alpha;
alpha_es=alpha;
alpha_se=alpha;
alpha_sr=alpha;
alpha_sn=alpha;
alpha_re=alpha;
alpha_rs=alpha;

% beta_ab
beta_ee=beta;
beta_ei=beta;
beta_es=beta;
beta_se=beta;
beta_sr=beta;
beta_sn=beta;
beta_re=beta;
beta_rs=beta;

% delays
taues=round(a*t0/deltat);  % thalamus to cortex
tause=round(t0/deltat)-taues;  % cortex to thalamus (so t0 is really round(t0/deltat))

% calculate initial conditions
r=0;%0.0001;%*(rand-0.5); % perturb ICs from fixed pt
ve=fzero(@ve_root,0.0005)+r;
phie=s(ve);
vr=nure*phie+(nurs/nues)*(ve-(nuee+nuei)*phie);
phir=s(vr);
vs=nuse*phie+nusr*phir+nusn*phin_ic;
phis=s(vs);

fid = fopen(confname,'w');

fprintf(fid,'A Configuration file for the code ''eegcode''\n');
fprintf(fid,'Nodes per population: 4\n');
fprintf(fid,'Number of neural populations: 6\n');
fprintf(fid,'Number of neural connections: 12\n');
fprintf(fid,'\n');
fprintf(fid,'Population connection matrix  1   2   3   4   5   6\n');
fprintf(fid,'1			     :1  :5  :0  :8  :0  :0\n');
fprintf(fid,'2			     :2  :6  :0  :9  :0  :0\n');
fprintf(fid,'3			     :3  :0  :0  :10 :0  :0\n');
fprintf(fid,'4			     :4  :0  :7  :0  :11 :12\n');
fprintf(fid,'5			     :0  :0  :0  :0  :0  :0\n');
fprintf(fid,'6			     :0  :0  :0  :0  :0  :0\n');
fprintf(fid,'Number of integration steps: %d Deltat: %f \n',nsteps,deltat);
fprintf(fid,'Q delay depths: %d : 0 : 0 : %d :0 :0\n',tause,taues);
fprintf(fid,'Propagator types 1: Waveeqn 2: Waveeqn 3: Waveeqn 4: Waveeqn 5: Mapping $\n');
fprintf(fid,'6: Mapping 7: Mapping 8: Mapping 9: Mapping 10: Mapping 11: Mapping 12: Mapping $\n');
fprintf(fid,'\n');
fprintf(fid,'Coupling types 1: Simple 2: Simple 3: Simple 4: Simple 5: Simple 6: Simple $\n');
fprintf(fid,'7: Simple 8: Simple 9: Simple 10: Simple 11: Simple 12: Simple $\n');
fprintf(fid,'\n');
fprintf(fid,'Population 1 - Excitory neurons\n');
fprintf(fid,' Initial Q: %f\n',phie);
fprintf(fid,' Firing response Theta: %f Sigma: %f Qmax: %f\n',theta,sigma,qmax);
fprintf(fid,' Number of Dendritic responses: 3\n');
fprintf(fid,'  Dendritic response from population 1 V initial: %f alpha: %f beta: %f\n',nuee*phie,alpha_ee,beta_ee);  % Initial V is actually Vab
fprintf(fid,'  Dendritic response from population 2 V initial: %f alpha: %f beta: %f\n',nuei*phie,alpha_ei,beta_ei);
fprintf(fid,'  Dendritic response from population 4 V initial: %f alpha: %f beta: %f\n',nues*phis,alpha_es,beta_es);
fprintf(fid,'\n');
fprintf(fid,'Population 2 - Inhibitory neurons\n');
fprintf(fid,'  Initial Q: %f\n',phie);
fprintf(fid,'  Firing response Theta: %f Sigma: %f Qmax: %f\n',theta,sigma,qmax);
fprintf(fid,'  Number of Dendritic responses: 3\n');
fprintf(fid,'   Dendritic response from population 1 V initial: %f alpha: %f beta: %f\n',nuee*phie,alpha_ee,beta_ee);
fprintf(fid,'   Dendritic response from population 2 V initial: %f alpha: %f beta: %f\n',nuei*phie,alpha_ei,beta_ei);
fprintf(fid,'   Dendritic response from population 4 V initial: %f alpha: %f beta: %f\n',nues*phis,alpha_es,beta_es);
fprintf(fid,'\n');
fprintf(fid,'Population 3 - Reticular nucleus neurons\n');
fprintf(fid,'  Initial Q: %f\n',phir);
fprintf(fid,'  Firing response Theta: %f Sigma: %f Qmax: %f\n',theta,sigma,qmax);
fprintf(fid,'  Number of Dendritic responses: 2\n');
fprintf(fid,'   Dendritic response from population 1 V initial: %f alpha: %f beta: %f\n',nure*phie,alpha_re,beta_re);
fprintf(fid,'   Dendritic response from population 4 V initial: %f alpha: %f beta: %f\n',nurs*phis,alpha_rs,beta_rs);
fprintf(fid,'\n');
fprintf(fid,'Population 4 - Relay nuclei neurons\n');
fprintf(fid,'  Initial Q: %f\n',phis);
fprintf(fid,'  Firing response Theta: %f Sigma: %f Qmax: %f\n',theta,sigma,qmax);
fprintf(fid,'  Number of Dendritic responses: 4\n');
fprintf(fid,'   Dendritic response from population 1 V initial: %f alpha: %f beta: %f\n',nuse*phie,alpha_se,beta_se);
fprintf(fid,'   Dendritic response from population 3 V initial: %f alpha: %f beta: %f\n',nusr*phir,alpha_sr,beta_sr);
fprintf(fid,'   Dendritic response from population 5 V initial: %f alpha: %f beta: %f\n',nusn*phin_ic,alpha_sn,beta_sn); % changed from phi_n(0)==0
fprintf(fid,'   Dendritic response from population 6 V initial: %f alpha: %f beta: %f\n',nusn*(whichstim==1),alpha_sn,beta_sn); % i.e., 1 for square, 0 for sine
fprintf(fid,'\n');
fprintf(fid,'Population 5 - Stimulus neurons - background phin\n');
fprintf(fid,'  Initial Q: %f\n',phin_ic);  
fprintf(fid,'  Stimulus mode: 4 Time to start of stimulus: 0.0 Amplitude: %f Mean: %f\n',noiseamp,phin_ic); 

fprintf(fid,'Population 6 - Stimulus neurons - SSEP phin\n');
switch whichstim
    case 1,  % square
        fprintf(fid,'  Initial Q: %f\n',1);  % CHECK initial Q
        fprintf(fid,'  Stimulus mode: 1 Time to start of stimulus: %f Amplitude: %f Pulse Duration: %f Pulse repetition period: %f\n',stimstart,stimamp,1/stimfreq/2,1/stimfreq);  
    case 3,  % sine
        fprintf(fid,'  Initial Q: %f\n',0);  % CHECK initial Q
        fprintf(fid,'  Stimulus mode: 3 Time to start of stimulus: %f Amplitude: %f Modulation Frequency: %f\n',stimstart,stimamp,stimfreq);  
    otherwise,
        error('no such SSEP stimulus whichstim=%d',whichstim)
end

fprintf(fid,'Propagation data\n');
fprintf(fid,'Propagator 1  - Initial Phi: %f Deltax: %f Tauab: 0   Effective range: %f gamma: %f\n',phie,deltax,re,gammae);
fprintf(fid,'Propagator 2  - Initial Phi: %f Deltax: %f Tauab: 0   Effective range: %f gamma: %f\n',phie,deltax,re,gammae);
fprintf(fid,'Propagator 3  - Initial Phi: %f Deltax: %f Tauab: %d Effective range: %f gamma: %f\n',phie,deltax,tause,re,gammae);
fprintf(fid,'Propagator 4  - Initial Phi: %f Deltax: %f Tauab: %d Effective range: %f gamma: %f\n',phie,deltax,tause,re,gammae);
fprintf(fid,'Propagator 5  - Tauab: 0   \n');
fprintf(fid,'Propagator 6  - Tauab: 0   \n');
fprintf(fid,'Propagator 7  - Tauab: 0   \n');
fprintf(fid,'Propagator 8  - Tauab: %d \n',taues);  % why wave eqn for phis here?
fprintf(fid,'Propagator 9  - Tauab: %d \n',taues);  %  ==> changing to mapping
fprintf(fid,'Propagator 10 - Tauab: 0   \n');
fprintf(fid,'Propagator 11 - Tauab: 0   \n');
fprintf(fid,'Propagator 12 - Tauab: 0   \n');
fprintf(fid,'Coupling data 1  - Nu: %f\n',nuee);
fprintf(fid,'Coupling data 2  - Nu: %f\n',nuee);
fprintf(fid,'Coupling data 3  - Nu: %f\n',nure);
fprintf(fid,'Coupling data 4  - Nu: %f\n',nuse);
fprintf(fid,'Coupling data 5  - Nu: %f\n',nuei);
fprintf(fid,'Coupling data 6  - Nu: %f\n',nuei);
fprintf(fid,'Coupling data 7  - Nu: %f\n',nusr);
fprintf(fid,'Coupling data 8  - Nu: %f\n',nues);
fprintf(fid,'Coupling data 9  - Nu: %f\n',nues);
fprintf(fid,'Coupling data 10 - Nu: %f\n',nurs);
fprintf(fid,'Coupling data 11 - Nu: %f\n',nusn);
fprintf(fid,'Coupling data 12 - Nu: %f\n',nusn);  % new param, how to handle nusn and amplitude?
fprintf(fid,'\n');
fprintf(fid,'Output Data - Number of traces : 5 \n');
fprintf(fid,'Wave Equation Number :1 Single/All: Single node number :1\n'); % phie
fprintf(fid,'Wave Equation Number :5 Single/All: Single node number :1\n'); % phii
fprintf(fid,'Wave Equation Number :7 Single/All: Single node number :1\n'); % phir
fprintf(fid,'Wave Equation Number :10 Single/All: Single node number :1\n'); % phis - the phis going into Vr
fprintf(fid,'Wave Equation Number :12 Single/All: Single node number :1\n'); % phin SSEP



fclose(fid);


function out=s(v)
% sigmoid function
% uses theta, sig, and qmax from above
    out=qmax./(1+exp((theta-v)/sigma));
end

function out=ve_root(ve)
% from ve_root.m
    out=s(nuse*s(ve)+nusr*s(nure*s(ve)+(nurs/nues)*(ve-(nuee+nuei)*s(ve)))+nusn*phin_ic)+((nuee+nuei)*s(ve)-ve)/nues;
end

end