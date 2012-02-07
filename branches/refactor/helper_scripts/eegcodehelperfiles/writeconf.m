% write eegcode.conf

%%%%%%%%%%%%%%%%%%%
% TODO: - TEST
%       - more sophisticated root finding to guard against bad ICs
%       - parametrize noise drive as in ssepconf



default=3;

fid = fopen('eegcode.conf','w');

% for input of stimulus type:
% case 1: //  Pulse stimulus pattern 
%     Stimulus mode: 1 Time to start of stimulus: 0.0002 Amplitude: 20 Pulse Duration: 20 Pulse repetition period: 20   % OR: '...Pulse Duration: %d Pulse repetition period: %d\n',floor(2*nsteps*deltat),floor(2*nsteps*deltat)
% case 2: //  White noise stimulus pattern 
%     Stimulus mode: 1 Time to start of stimulus: 0.0002 Amplitude: 20 Mean: 5
% case 3: //  Sinusoidal stimulus pattern 
%     Stimulus mode: 1 Time to start of stimulus: 0.0002 Amplitude: 20 Modulation Frequency: 10
% case 4: //  Coherent white noise stimulus pattern (spatially coherent)
%     Stimulus mode: 1 Time to start of stimulus: 0.0002 Amplitude: 20 Mean: 5

% eegcode default, 4 nodes
if default==1,
    fprintf(fid,'A Configuration file for the code ''eegcode''\n');
    fprintf(fid,'Nodes per population: 4\n');
    fprintf(fid,'Number of neural populations: 5\n');
    fprintf(fid,'Number of neural connections: 11\n');
    fprintf(fid,'\n');
    fprintf(fid,'Population connection matrix  1   2   3   4   5\n');
    fprintf(fid,'1			     :1  :5  :0  :8  :0\n');
    fprintf(fid,'2			     :2  :6  :0  :9  :0\n');
    fprintf(fid,'3			     :3  :0  :0  :10 :0\n');
    fprintf(fid,'4			     :4  :0  :7  :0  :11\n');
    fprintf(fid,'5			     :0  :0  :0  :0  :0 \n');
    fprintf(fid,'Number of integration steps: 100000 Deltat: 0.0001 \n');
    fprintf(fid,'Q delay depths: 420 : 0 : 0 : 420 :0\n');
    fprintf(fid,'Propagator types 1: Waveeqn 2: Waveeqn 3: Waveeqn 4: Waveeqn 5: Mapping $\n');
    fprintf(fid,'6: Mapping 7: Mapping 8: Waveeqn 9: Waveeqn 10: Mapping 11: Mapping $\n');
    fprintf(fid,'\n');
    fprintf(fid,'Coupling types 1: Simple 2: Simple 3: Simple 4: Simple 5: Simple 6: Simple $\n');
    fprintf(fid,'7: Simple 8: Simple 9: Simple 10: Simple 11: Simple $\n');
    fprintf(fid,'\n');
    fprintf(fid,'Population 1 - Excitory neurons\n');
    fprintf(fid,' Initial Q: 8.87145\n');
    fprintf(fid,' Firing response Theta: 0.013 Sigma: 0.0038 Qmax: 250.0\n');
    fprintf(fid,' Number of Dendritic responses: 3\n');
    fprintf(fid,'  Dendritic response from population 1 V initial: 0.0106457 alpha: 75.0 beta: 285.0 \n');
    fprintf(fid,'  Dendritic response from population 2 V initial:-0.0159686 alpha: 75.0 beta: 285.0\n');
    fprintf(fid,'  Dendritic response from population 4 V initial: 0.0057734 alpha: 75.0 beta: 285.0\n');
    fprintf(fid,'\n');
    fprintf(fid,'Population 2 - Inhibitory neurons\n');
    fprintf(fid,'  Initial Q: 8.87145\n');
    fprintf(fid,'  Firing response Theta: 0.013 Sigma: 0.0038 Qmax: 250.0\n');
    fprintf(fid,'  Number of Dendritic responses: 3\n');
    fprintf(fid,'   Dendritic response from population 1 V initial: 0.0106457 alpha: 75.0 beta: 285.0\n');
    fprintf(fid,'   Dendritic response from population 2 V initial:-0.0159686 alpha: 75.0 beta: 285.0\n');
    fprintf(fid,'   Dendritic response from population 4 V initial: 0.0057734 alpha: 75.0 beta: 285.0\n');
    fprintf(fid,'\n');
    fprintf(fid,'Population 3 - Reticular nucleus neurons\n');
    fprintf(fid,'  Initial Q: 15.7298\n');
    fprintf(fid,'  Firing response Theta: 0.013 Sigma: 0.0038 Qmax: 250.0\n');
    fprintf(fid,'  Number of Dendritic responses: 2\n');
    fprintf(fid,'   Dendritic response from population 1 V initial: 0.00177429 alpha: 75.0 beta: 285.0\n');
    fprintf(fid,'   Dendritic response from population 4 V initial: 0.000962233 alpha: 75.0 beta: 285.0\n');
    fprintf(fid,'\n');
    fprintf(fid,'Population 4 - Relay nuclei neurons\n');
    fprintf(fid,'  Initial Q: 4.81117\n');
    fprintf(fid,'  Firing response Theta: 0.013 Sigma: 0.0038 Qmax: 250.0\n');
    fprintf(fid,'  Number of Dendritic responses: 3\n');
    fprintf(fid,'   Dendritic response from population 1 V initial: 0.0106457 alpha: 75.0 beta: 285.0\n');
    fprintf(fid,'   Dendritic response from population 3 V initial:-0.0125839 alpha: 75.0 beta: 285.0\n');
    fprintf(fid,'   Dendritic response from population 5 V initial: 0.00	  alpha: 75.0 beta: 285.0\n');
    fprintf(fid,'\n');
    fprintf(fid,'Population 5 - Stimulus neurons\n');
    fprintf(fid,'  Initial Q: 0.0\n');
    fprintf(fid,'  Stimulus mode: 4 Time to start of stimulus: 0.0002 Amplitude: 0 Mean: 5\n'); % 2,5
    fprintf(fid,'Propagation data\n');
    fprintf(fid,'Propagator 1  - Initial Phi: 8.87145 Deltax: 0.0035 Tauab: 0   Effective range: 0.08 gamma: 100.0\n');
    fprintf(fid,'Propagator 2  - Initial Phi: 8.87145 Deltax: 0.0035 Tauab: 0   Effective range: 0.08 gamma: 100.0\n');
    fprintf(fid,'Propagator 3  - Initial Phi: 8.87145 Deltax: 0.0035 Tauab: 420 Effective range: 0.08 gamma: 100.0\n');
    fprintf(fid,'Propagator 4  - Initial Phi: 8.87145 Deltax: 0.0035 Tauab: 420 Effective range: 0.08 gamma: 100.0\n');
    fprintf(fid,'Propagator 5  - Tauab: 0   \n');
    fprintf(fid,'Propagator 6  - Tauab: 0   \n');
    fprintf(fid,'Propagator 7  - Tauab: 0   \n');
    fprintf(fid,'Propagator 8  - Initial Phi: 4.81117 Deltax: 0.0035 Tauab: 420 Effective range: 0.08 gamma: 100.0\n');
    fprintf(fid,'Propagator 9  - Initial Phi: 4.81117 Deltax: 0.0035 Tauab: 420 Effective range: 0.08 gamma: 100.0\n');
    fprintf(fid,'Propagator 10 - Tauab: 0   \n');
    fprintf(fid,'Propagator 11 - Tauab: 0   \n');
    fprintf(fid,'Coupling data 1  - Nu: 0.0012\n');
    fprintf(fid,'Coupling data 2  - Nu: 0.0012\n');
    fprintf(fid,'Coupling data 3  - Nu: 0.0002\n');
    fprintf(fid,'Coupling data 4  - Nu: 0.0012\n');
    fprintf(fid,'Coupling data 5  - Nu:-0.0018\n');
    fprintf(fid,'Coupling data 6  - Nu:-0.0018\n');
    fprintf(fid,'Coupling data 7  - Nu:-0.0008\n');
    fprintf(fid,'Coupling data 8  - Nu: 0.0012\n');
    fprintf(fid,'Coupling data 9  - Nu: 0.0012\n');
    fprintf(fid,'Coupling data 10 - Nu: 0.0002\n');
    fprintf(fid,'Coupling data 11 - Nu: 0.00015\n');
    fprintf(fid,'\n');
    fprintf(fid,'Output Data - Number of traces : 4 \n');
    fprintf(fid,'Wave Equation Number :1 Single/All: Single node number :1\n');
    fprintf(fid,'Wave Equation Number :5 Single/All: Single node number :1\n');
    fprintf(fid,'Wave Equation Number :6 Single/All: Single node number :1\n');
    fprintf(fid,'Wave Equation Number :8 Single/All: Single node number :1\n');
end

% parsetec2, 4 nodes, same ICs as above
if default==2,
    fprintf(fid,'A Configuration file for the code ''eegcode''\n');
    fprintf(fid,'Nodes per population: 4\n');
    fprintf(fid,'Number of neural populations: 5\n');
    fprintf(fid,'Number of neural connections: 11\n');
    fprintf(fid,'\n');
    fprintf(fid,'Population connection matrix  1   2   3   4   5\n');
    fprintf(fid,'1			     :1  :5  :0  :8  :0\n');
    fprintf(fid,'2			     :2  :6  :0  :9  :0\n');
    fprintf(fid,'3			     :3  :0  :0  :10 :0\n');
    fprintf(fid,'4			     :4  :0  :7  :0  :11\n');
    fprintf(fid,'5			     :0  :0  :0  :0  :0 \n');
    fprintf(fid,'Number of integration steps: 100000 Deltat: 0.0001 \n');
    fprintf(fid,'Q delay depths: 400 : 0 : 0 : 400 :0\n');
    fprintf(fid,'Propagator types 1: Waveeqn 2: Waveeqn 3: Waveeqn 4: Waveeqn 5: Mapping $\n');
    fprintf(fid,'6: Mapping 7: Mapping 8: Waveeqn 9: Waveeqn 10: Mapping 11: Mapping $\n');
    fprintf(fid,'\n');
    fprintf(fid,'Coupling types 1: Simple 2: Simple 3: Simple 4: Simple 5: Simple 6: Simple $\n');
    fprintf(fid,'7: Simple 8: Simple 9: Simple 10: Simple 11: Simple $\n');
    fprintf(fid,'\n');
    fprintf(fid,'Population 1 - Excitory neurons\n');
    fprintf(fid,' Initial Q: 8.87145\n');
    fprintf(fid,' Firing response Theta: 0.015 Sigma: 0.0033 Qmax: 250.0\n');
    fprintf(fid,' Number of Dendritic responses: 3\n');
    fprintf(fid,'  Dendritic response from population 1 V initial: 0.0106457 alpha: 60.0 beta: 240.0 \n');
    fprintf(fid,'  Dendritic response from population 2 V initial:-0.0159686 alpha: 60.0 beta: 240.0\n');
    fprintf(fid,'  Dendritic response from population 4 V initial: 0.0057734 alpha: 60.0 beta: 240.0\n');
    fprintf(fid,'\n');
    fprintf(fid,'Population 2 - Inhibitory neurons\n');
    fprintf(fid,'  Initial Q: 8.87145\n');
    fprintf(fid,'  Firing response Theta: 0.015 Sigma: 0.0033 Qmax: 250.0\n');
    fprintf(fid,'  Number of Dendritic responses: 3\n');
    fprintf(fid,'   Dendritic response from population 1 V initial: 0.0106457 alpha: 60.0 beta: 240.0\n');
    fprintf(fid,'   Dendritic response from population 2 V initial:-0.0159686 alpha: 60.0 beta: 240.0\n');
    fprintf(fid,'   Dendritic response from population 4 V initial: 0.0057734 alpha: 60.0 beta: 240.0\n');
    fprintf(fid,'\n');
    fprintf(fid,'Population 3 - Reticular nucleus neurons\n');
    fprintf(fid,'  Initial Q: 15.7298\n');
    fprintf(fid,'  Firing response Theta: 0.015 Sigma: 0.0033 Qmax: 250.0\n');
    fprintf(fid,'  Number of Dendritic responses: 2\n');
    fprintf(fid,'   Dendritic response from population 1 V initial: 0.00177429 alpha: 60.0 beta: 240.0\n');
    fprintf(fid,'   Dendritic response from population 4 V initial: 0.000962233 alpha: 60.0 beta: 240.0\n');
    fprintf(fid,'\n');
    fprintf(fid,'Population 4 - Relay nuclei neurons\n');
    fprintf(fid,'  Initial Q: 4.81117\n');
    fprintf(fid,'  Firing response Theta: 0.015 Sigma: 0.0033 Qmax: 250.0\n');
    fprintf(fid,'  Number of Dendritic responses: 3\n');
    fprintf(fid,'   Dendritic response from population 1 V initial: 0.0106457 alpha: 60.0 beta: 240.0\n');
    fprintf(fid,'   Dendritic response from population 3 V initial:-0.0125839 alpha: 60.0 beta: 240.0\n');
    fprintf(fid,'   Dendritic response from population 5 V initial: 0.00	  alpha: 60.0 beta: 240.0\n');
    fprintf(fid,'\n');
    fprintf(fid,'Population 5 - Stimulus neurons\n');
    fprintf(fid,'  Initial Q: 0.0\n');
    fprintf(fid,'  Stimulus mode: 1 Time to start of stimulus: 0.0001 Amplitude: 1 Pulse Duration: 20 Pulse repetition period: 20\n');
    fprintf(fid,'Propagation data\n');
    fprintf(fid,'Propagator 1  - Initial Phi: 8.87145 Deltax: 0.0035 Tauab: 0   Effective range: 0.08 gamma: 100.0\n');
    fprintf(fid,'Propagator 2  - Initial Phi: 8.87145 Deltax: 0.0035 Tauab: 0   Effective range: 0.08 gamma: 100.0\n');
    fprintf(fid,'Propagator 3  - Initial Phi: 8.87145 Deltax: 0.0035 Tauab: 400 Effective range: 0.08 gamma: 100.0\n');
    fprintf(fid,'Propagator 4  - Initial Phi: 8.87145 Deltax: 0.0035 Tauab: 400 Effective range: 0.08 gamma: 100.0\n');
    fprintf(fid,'Propagator 5  - Tauab: 0   \n');
    fprintf(fid,'Propagator 6  - Tauab: 0   \n');
    fprintf(fid,'Propagator 7  - Tauab: 0   \n');
    fprintf(fid,'Propagator 8  - Initial Phi: 4.81117 Deltax: 0.0035 Tauab: 400 Effective range: 0.08 gamma: 100.0\n');
    fprintf(fid,'Propagator 9  - Initial Phi: 4.81117 Deltax: 0.0035 Tauab: 400 Effective range: 0.08 gamma: 100.0\n');
    fprintf(fid,'Propagator 10 - Tauab: 0   \n');
    fprintf(fid,'Propagator 11 - Tauab: 0   \n');
    fprintf(fid,'Coupling data 1  - Nu: 0.0012\n');
    fprintf(fid,'Coupling data 2  - Nu: 0.0012\n');
    fprintf(fid,'Coupling data 3  - Nu: 0.0002\n');
    fprintf(fid,'Coupling data 4  - Nu: 0.0010\n');
    fprintf(fid,'Coupling data 5  - Nu:-0.0018\n');
    fprintf(fid,'Coupling data 6  - Nu:-0.0018\n');
    fprintf(fid,'Coupling data 7  - Nu:-0.0010\n');
    fprintf(fid,'Coupling data 8  - Nu: 0.0014\n');
    fprintf(fid,'Coupling data 9  - Nu: 0.0014\n');
    fprintf(fid,'Coupling data 10 - Nu: 0.0002\n');
    fprintf(fid,'Coupling data 11 - Nu: 0.0010\n');
    fprintf(fid,'\n');
    fprintf(fid,'Output Data - Number of traces : 4 \n');
    fprintf(fid,'Wave Equation Number :1 Single/All: Single node number :1\n');
    fprintf(fid,'Wave Equation Number :5 Single/All: Single node number :1\n');
    fprintf(fid,'Wave Equation Number :7 Single/All: Single node number :1\n');
    fprintf(fid,'Wave Equation Number :8 Single/All: Single node number :1\n');
end

% params and ICs from parset
if default==3,
    % integration data
    nsteps=100000; %6*60*2000;
    deltat=0.0001; % 0.0005
    
    global par    % for storing params in to pass elsewhere more easily, numbered as in matlab code
    
    parset=1;  % 1==parsetect0asym, 2==parsetpmt0asym, 3==defaults from above
    
    if parset==1,  % from parsetect0asym
        a=0.5; par(1)=a;                                 %a=0.5  % 0<a<1, proportion thalamus->cortex
        theta=0.015; par(4)=theta;                       %theta=0.015
        sig0=0.006; par(7)=sig0; sigma=sig0*sqrt(3)/pi;  %sig0=0.006  % alt could define sigma, but matlab code uses sig0
        qmax=250; par(8)=qmax;                           %qmax=250
        re=0.08; par(10)=re;                             %re=0.08
        gammae=100; par(12)=gammae;                      %gammae=100
        alpha=100; par(13)=alpha;                         %alpha=60
        beta=1*alpha; par(14)=beta;                      %beta=4*alpha
        t0=0.066; par(15)=t0;                             %t0=0.08
        nuee=0e-4; par(20)=nuee;                        %nuee=12e-4
        nuei=0e-4; par(21)=nuei;                       %nuei=-18e-4
        nues=14.8e-4; par(22)=nues;                        %nues=14e-4
        nuse=0.0e-4; par(23)=nuse;                        %nuse=10e-4
        nusr=-11.6e-4; par(24)=nusr;                       %nusr=-10e-4
        nusn=184e-4; par(25)=nusn;                        %nusn=10e-4  % ==nusn*phin
        nure=3.99e-4; par(26)=nure;                         %nure=2e-4
        nurs=13.1e-4; par(27)=nurs;                         %nurs=2e-4
        deltax=0.0035; par(28)=deltax;                   %deltax=0.0035
    end
    
    if parset==2,  % from parsetpmt0asym
        a=0.5; par(1)=a;                                 %a=0.5  % 0<a<1, proportion thalamus->cortex
        theta=0.015; par(4)=theta;                       %theta=0.015
        sig0=0.006; par(7)=sig0; sigma=sig0*sqrt(3)/pi;  %sig0=0.006  % alt could define sigma, but matlab code uses sig0
        qmax=250; par(8)=qmax;                           %qmax=250
        re=0.08; par(10)=re;                             %re=0.08
        gammae=100; par(12)=gammae;                      %gammae=100
        alpha=50; par(13)=alpha;                         %alpha=60
        beta=4*alpha; par(14)=beta;                      %beta=4*alpha
        t0=0.08; par(15)=t0;                             %t0=0.08
        nuee=10e-4; par(20)=nuee;                        %nuee=10e-4
        nuei=-18e-4; par(21)=nuei;                       %nuei=-18e-4
        nues=32e-4; par(22)=nues;                        %nues=32e-4
        nuse=44e-4; par(23)=nuse;                        %nuse=44e-4
        nusr=-8e-4; par(24)=nusr;                        %nusr=-8e-4
        nusn=20e-4; par(25)=nusn;                        %nusn=20e-4  % ==nusn*phin
        nure=16e-4; par(26)=nure;                        %nure=16e-4
        nurs=6e-4; par(27)=nurs;                         %nurs=6e-4
        deltax=0.0035; par(28)=deltax;                   %deltax=0.0035
    end
  
    if parset==3,  % from defaults above
        a=0.5; par(1)=a;                                 %a=0.5  % 0<a<1, proportion thalamus->cortex
        theta=0.013; par(4)=theta;                       %theta=0.013
        sigma=0.0038; sig0=sigma*pi/sqrt(3); par(7)=sig0;%sigma=0.0038
        qmax=250; par(8)=qmax;                           %qmax=250
        re=0.08; par(10)=re;                             %re=0.08
        gammae=100; par(12)=gammae;                      %gammae=100
        alpha=75; par(13)=alpha;                         %alpha=75
        beta=285; par(14)=beta;                          %beta=285
        t0=0.084; par(15)=t0;                            %t0=0.084
        nuee=12e-4; par(20)=nuee;                        %nuee=12e-4
        nuei=-18e-4; par(21)=nuei;                       %nuei=-18e-4
        nues=12e-4; par(22)=nues;                        %nues=12e-4
        nuse=12e-4; par(23)=nuse;                        %nuse=12e-4
        nusr=-8e-4; par(24)=nusr;                        %nusr=-8e-4
        nusn=0; par(25)=nusn;                            %nusn=0  % really phin==0
        nure=2e-4; par(26)=nure;                         %nure=2e-4
        nurs=2e-4; par(27)=nurs;                         %nurs=2e-4
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
    r=0.0001;%0.002;%*(rand-0.5); % perturb ICs from fixed pt
    ve=fzero(@ve_root,0.001)+r;
    phie=sigm(ve,theta,sig0,qmax);
    vr=nure*phie+(nurs/nues)*(ve-(nuee+nuei)*phie);
    phir=sigm(vr,theta,sig0,qmax);
    vs=nuse*phie+nusr*phir+nusn;
    phis=sigm(vs,theta,sig0,qmax);
    
    ves_ic=nues*phis;
    if nuei~=0,
        vei_ic=(ve-ves_ic)/(1+nuee/nuei);
        vee_ic=vei_ic*nuee/nuei;
    else
        vei_ic=0;
        vee_ic=ve-ves_ic;
    end
        
    vrs_ic=nurs*phis;
    vre_ic=vr-vrs_ic;
    
    noiseamp=0;
    
    % differs from the string used by sseploop in that nusnphin is not split here
    % and no sinusoidal drive
    titlestr={sprintf('alpha=%g  beta=%g  alpha\\_sr=%g  beta\\_sr=%g  gammae=%g  t0=%g',alpha_ee,beta_ee,alpha_sr,beta_sr,gammae,t0),...
          sprintf('nuee=%g  nuei=%g  nues=%g  nuse=%g  nusr=%g  nusn=%g  nure=%g  nurs=%g',nuee,nuei,nues,nuse,nusr,nusn,nure,nurs),...
          sprintf('phie\\_ic=%g  phis\\_ic=%g  phir\\_ic=%g  noiseamp=%g  nsteps=%d  deltat=%g',phie,phis,phir,noiseamp,nsteps,deltat)};

    
    if 0,
        rhoe=sigd(ve,theta,sig0,qmax);
        rhos=sigd(vs,theta,sig0,qmax);
        rhor=sigd(vr,theta,sig0,qmax);
        gee=rhoe*nuee;
        gei=rhoe*nuei;
        ges=rhoe*nues;
        gse=rhos*nuse;
        gsr=rhos*nusr;
        gsn=rhos*nusn;
        gre=rhor*nure;
        grs=rhor*nurs;
        gese=ges*gse;
        gesre=ges*gsr*gre;
        gsrs=gsr*grs;
        gesn=ges*gsn;
        xpar=gee./(1-gei);
        ypar=(gese+gesre)./((1-gei).*(1-gsrs));
        zpar=-gsrs.*alpha*beta/(alpha+beta)^2;
        fprintf(1,'gee=%f  gei=%f  ges=%f  gse=%f  gsr=%f  gsn=%f  gre=%f  grs=%f \n',gee,gei,ges,gse,gsr,gsn,gre,grs)
        fprintf(1,'gese=%f  gesre=%f  gsrs=%f  gesn=%f \n',gese,gesre,gsrs,gesn)
        fprintf(1,'x=%f  y=%f  z=%f \n',xpar,ypar,zpar)
        %v=linspace(-0.01,0.2,1000);
        %figure(200), plot(sigm(v,theta,sig0,qmax),ve_root(v),'b-',phie,0,'ro')
    end

    
    fprintf(fid,'A Configuration file for the code ''eegcode''\n');
    fprintf(fid,'Nodes per population: 4\n');
    fprintf(fid,'Number of neural populations: 5\n');
    fprintf(fid,'Number of neural connections: 11\n');
    fprintf(fid,'\n');
    fprintf(fid,'Population connection matrix  1   2   3   4   5\n');
    fprintf(fid,'1			     :1  :5  :0  :8  :0\n');
    fprintf(fid,'2			     :2  :6  :0  :9  :0\n');
    fprintf(fid,'3			     :3  :0  :0  :10 :0\n');
    fprintf(fid,'4			     :4  :0  :7  :0  :11\n');
    fprintf(fid,'5			     :0  :0  :0  :0  :0 \n');
    fprintf(fid,'Number of integration steps: %d Deltat: %f \n',nsteps,deltat);
    fprintf(fid,'Q delay depths: %d : 0 : 0 : %d :0\n',tause,taues);
    fprintf(fid,'Propagator types 1: Waveeqn 2: Waveeqn 3: Waveeqn 4: Waveeqn 5: Mapping $\n');
    fprintf(fid,'6: Mapping 7: Mapping 8: Mapping 9: Mapping 10: Mapping 11: Mapping $\n');
    fprintf(fid,'\n');
    fprintf(fid,'Coupling types 1: Simple 2: Simple 3: Simple 4: Simple 5: Simple 6: Simple $\n');
    fprintf(fid,'7: Simple 8: Simple 9: Simple 10: Simple 11: Simple $\n');
    fprintf(fid,'\n');
    fprintf(fid,'Population 1 - Excitory neurons\n');
    fprintf(fid,' Initial Q: %f\n',phie);
    fprintf(fid,' Firing response Theta: %f Sigma: %f Qmax: %f\n',theta,sigma,qmax);
    fprintf(fid,' Number of Dendritic responses: 3\n');
    fprintf(fid,'  Dendritic response from population 1 V initial: %f alpha: %f beta: %f\n',vee_ic,alpha_ee,beta_ee);  % Initial V is actually Vab
    fprintf(fid,'  Dendritic response from population 2 V initial: %f alpha: %f beta: %f\n',vei_ic,alpha_ei,beta_ei);
    fprintf(fid,'  Dendritic response from population 4 V initial: %f alpha: %f beta: %f\n',ves_ic,alpha_es,beta_es);
    fprintf(fid,'\n');
    fprintf(fid,'Population 2 - Inhibitory neurons\n');
    fprintf(fid,'  Initial Q: %f\n',phie);
    fprintf(fid,'  Firing response Theta: %f Sigma: %f Qmax: %f\n',theta,sigma,qmax);
    fprintf(fid,'  Number of Dendritic responses: 3\n');
    fprintf(fid,'   Dendritic response from population 1 V initial: %f alpha: %f beta: %f\n',vee_ic,alpha_ee,beta_ee);
    fprintf(fid,'   Dendritic response from population 2 V initial: %f alpha: %f beta: %f\n',vei_ic,alpha_ei,beta_ei);
    fprintf(fid,'   Dendritic response from population 4 V initial: %f alpha: %f beta: %f\n',ves_ic,alpha_es,beta_es);
    fprintf(fid,'\n');
    fprintf(fid,'Population 3 - Reticular nucleus neurons\n');
    fprintf(fid,'  Initial Q: %f\n',phir);
    fprintf(fid,'  Firing response Theta: %f Sigma: %f Qmax: %f\n',theta,sigma,qmax);
    fprintf(fid,'  Number of Dendritic responses: 2\n');
    fprintf(fid,'   Dendritic response from population 1 V initial: %f alpha: %f beta: %f\n',vre_ic,alpha_re,beta_re);
    fprintf(fid,'   Dendritic response from population 4 V initial: %f alpha: %f beta: %f\n',vrs_ic,alpha_rs,beta_rs);
    fprintf(fid,'\n');
    fprintf(fid,'Population 4 - Relay nuclei neurons\n');
    fprintf(fid,'  Initial Q: %f\n',phis);
    fprintf(fid,'  Firing response Theta: %f Sigma: %f Qmax: %f\n',theta,sigma,qmax);
    fprintf(fid,'  Number of Dendritic responses: 3\n');
    fprintf(fid,'   Dendritic response from population 1 V initial: %f alpha: %f beta: %f\n',nuse*phie,alpha_se,beta_se);
    fprintf(fid,'   Dendritic response from population 3 V initial: %f alpha: %f beta: %f\n',nusr*phir,alpha_sr,beta_sr);
    fprintf(fid,'   Dendritic response from population 5 V initial: %f alpha: %f beta: %f\n',nusn*1,alpha_sn,beta_sn); % changed from phi_n(0)==0
    fprintf(fid,'\n');
    fprintf(fid,'Population 5 - Stimulus neurons\n');
    fprintf(fid,'  Initial Q: %f\n',1);  % changed from 0
    fprintf(fid,'  Stimulus mode: 4 Time to start of stimulus: 0.0 Amplitude: %f Mean: 1\n',noiseamp);  % changed to start at t=0
    %fprintf(fid,'  Stimulus mode: 1 Time to start of stimulus: 0.0 Amplitude: 1 Pulse Duration: %d Pulse repetition period: %d\n',floor(2*nsteps*deltat),floor(2*nsteps*deltat));  % changed to start at t=0
    %fprintf(fid,'  Stimulus mode: 1 Time to start of stimulus: 0.0 Amplitude: 1 Pulse Duration: %d Pulse repetition period: %d\n',0.025,0.05); 
    %fprintf(fid,'  Stimulus mode: 4 Time to start of stimulus: 0.0 Amplitude: %f Mean: %f',0.1,1);
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
    fprintf(fid,'\n');
    fprintf(fid,'Output Data - Number of traces : 4 \n');
    fprintf(fid,'Wave Equation Number :1 Single/All: Single node number :1\n'); % phie
    fprintf(fid,'Wave Equation Number :5 Single/All: Single node number :1\n'); % phii
    fprintf(fid,'Wave Equation Number :7 Single/All: Single node number :1\n'); % phir
    fprintf(fid,'Wave Equation Number :10 Single/All: Single node number :1\n'); % phis - the phis going into Vr
end



fclose(fid);
