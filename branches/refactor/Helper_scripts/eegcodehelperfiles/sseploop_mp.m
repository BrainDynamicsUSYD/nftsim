 % sseploop_mp - call ssepconf over a range of stim frequencies
 %             - multi processor version 

if(~matlabpool('size')),
    matlabpool
end

 
stimfreqs=0.5:0.5:30;
stimamp=0.1;%10*0.5;

nsteps=600000; %500000
deltat=0.0001;
intparams=[nsteps,deltat];

winlen=10;  %20
discard=20; %10

wel_window=winlen/deltat;  
wel_noverlap=[];  % default is 50% overlap
wel_nfft=wel_window;
wel_Fs=1/deltat;
Pwel=zeros(wel_nfft/2+1,length(stimfreqs));
fwel=zeros(size(Pwel));

specn=floor(winlen/deltat)/2;

%P=zeros(specn,length(stimfreqs));
%f=zeros(specn,length(stimfreqs));  % wasteful; just to avoid temp variable
Y=zeros(nsteps,length(stimfreqs));
%Pref=zeros(specn,1);

fprintf(1,'%d total \n',length(stimfreqs))



parfor j=1:length(stimfreqs),
    %fprintf(1,'%d ',j)
    %if ~mod(j,30), fprintf(1,'\n'), end
    
    confname=['eegcode.conf.ssep_' num2str(j)];
    ssepconf4([stimfreqs(j),stimamp],intparams,confname);
    outname=['eegcode.output.ssep_' num2str(j)];
    dumpname=['eegcode.dump.ssep_' num2str(j)];
    system(['eeg -i ' confname ' -o ' outname ' -d ' dumpname ]);
    
    [t,y]=readoutput2(outname);   
    
    %[P(:,j),f(:,j)]=pspec(y(:,1),deltat,discard,winlen);
    [Pwel(:,j),fwel(:,j)]=pwelch(y(:,1),wel_window,wel_noverlap,wel_nfft,wel_Fs);
    
    %[Pwel(:,j),fwel(:,j)]=pwelch(Y(:,j),20000,[],20000,10000);
    
        
    if 1,  % parfor takes issue with indexing P in here
    Y(:,j)=y(:,1); % store each phie
    %figure(7)  % plots of timeseries and spectra for each stimfreq
    %subplot(6,4,2*j-1), plot(t,y(:,1)), xlim([59 60]), title(sprintf('f_D=%g',stimfreqs(j)))
    %subplot(6,4,2*j), plot(f(1:maxfi),log10(P(1:maxfi,j))), xlim([0 50])
    end
    
    
    if j<length(stimfreqs),  % keep last one
        delete(confname,outname,dumpname)
    end
end
fprintf(1,'\n')

confname=['eegcode.conf.ssep_' num2str(length(stimfreqs))];
%f=f(:,1); % only want 1 f
fwel=fwel(:,1); % only want 1 f

% % get reference spectrum with 0 drive amplitude
% ssepconf([1 0],intparams); % 0 ampl; freq doesn't matter
% !eeg
% readoutput
% [Pref(:,1),f]=pspec(y(:,1),deltat,discard,winlen);

% read in pars
p=readpars(confname);
titlestr={sprintf('alpha=%g  beta=%g  alpha\\_sr=%g  beta\\_sr=%g  gammae=%g  t0=%g',p.alpha_ee,p.beta_ee,p.alpha_sr,p.beta_sr,p.gammae,p.t0),...
          sprintf('nuee=%g  nuei=%g  nues=%g  nuse=%g  nusr=%g  nusn=%g  nure=%g  nurs=%g',p.nus),...
          sprintf('phie\\_ic=%g  phis\\_ic=%g  phir\\_ic=%g  phin\\_ic=%g',p.phie_ic,p.phis_ic,p.phir_ic,p.phin_ic),...
          sprintf('noiseamp=%g  stimamp=%g  nsteps=%d  deltat=%g  winlen=%g  discard=%g',p.noiseamp,stimamp,nsteps,deltat,winlen,discard)};

%maxfi=find(f>=100, 1 );
maxfi=find(fwel>=100, 1 );

% Psmooth=zeros(size(P));
% Psmooth(2:end-1,:)=(P(2:end-1,:)+(P(1:end-2,:)+P(3:end,:))/2)/2;
% Psmooth(1,:)=P(1,:); Psmooth(end,:)=P(end,:);

% for recalculation
%clear Pwel fwel, for j=1:length(stimfreqs), [Pwel(:,j),fwel]=pwelch(Y(:,j),20000,[],20000,10000); end
      
% plots
if length(stimfreqs)>1,
    %figure(3), pcolor(stimfreqs,f(1:maxfi),log(P(1:maxfi,:))), shading flat, ylim([0 max(stimfreqs)]), xlabel('Stimulus f (Hz)'), ylabel('Response f (Hz)')
    %title(titlestr)
    
    %figure(4), plot(f(1:maxfi),log10(P(1:maxfi,:))), xlim([0 100])
    
    %figure(5), surf(stimfreqs,f,log(abs(Psmooth-Pref*ones(1,size(Psmooth,2))))), shading flat, view(2), ylim([0 max(stimfreqs)]), xlabel('Stimulus f (Hz)'), ylabel('Response f (Hz)')
    %figure(5), pcolor(stimfreqs,f(1:maxfi),log10(Psmooth(1:maxfi,:))), shading flat, ylim([0 max(stimfreqs)]), xlabel('Stimulus f (Hz)'), ylabel('Response f (Hz)')
    figure(5), pcolor(stimfreqs,fwel(1:maxfi),log10(Pwel(1:maxfi,:))), shading flat, ylim([0 max(stimfreqs)]), xlabel('Stimulus f (Hz)'), ylabel('Response f (Hz)')
    title(titlestr)
end
%figure(3), surf(stimfreqs,f,log(abs(P-P(:,end)*ones(1,size(P,2))))), shading flat, view(2), ylim([0 100])


% extra plots
if 0,
    % movie showing spectra for each stimfreq
    figure
    for j=1:length(stimfreqs),
        plot(f(1:maxfi),log10(P(1:maxfi,j)),'b',stimfreqs(j)*ones(1,2),[-5 10],'r--')
        xlim([0 50]), ylim([-5 10])
        Pmov(j)=getframe;
    end
    movie(Pmov)
    
    % polar coords
    [mesh_stimfreqs mesh_f]=meshgrid(stimfreqs,f(1:maxfi));
    [th rh Z]=cart2pol(mesh_stimfreqs,mesh_f,log10(Psmooth(1:maxfi,:)));
    figure
    pcolor(th/pi,rh,Z), shading flat
    xlabel('\theta/\pi')
    ylabel('sqrt(f^2+f_D^2) (Hz)')
end