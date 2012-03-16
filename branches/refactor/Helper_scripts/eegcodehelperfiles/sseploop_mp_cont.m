 % sseploop_mp_cont - call ssepconf4_2 over a range of stimamps
 %                  - continues from previous value
 %                  - NOT mp

% if(~matlabpool('size')),
%     matlabpool
% end
 
stimfreq=20;
stimamps=[0.04:0.04:10]-0.02;%10*0.5;

plotstring='k.';

clearfig=1;
if clearfig,
    figure(50), clf
end

reverse=0;
if reverse,
    stimamps=fliplr(stimamps);
    plotstring='r.';
    figure(50), hold on
end

nsteps=800000; %500000
deltat=0.0001;
intparams=[nsteps,deltat];

winlen=20;  %20
discard=20; %10

specn=floor(winlen/deltat)/2;
keepYn=floor(10/deltat);

P=zeros(specn,length(stimamps));
f=zeros(specn,length(stimamps));  % wasteful; just to avoid temp variable
Y=zeros(keepYn,length(stimamps));  % was nsteps; now just keep winlen/2
Pref=zeros(specn,1);

% if noiseamp=0, optionally calculate local extrema for bifdiags
if 1,
    bifpts=128;
    maxima=zeros(bifpts,length(stimamps));
    minima=zeros(bifpts,length(stimamps));
end



fprintf(1,'%d total \n',length(stimamps))
fprintf(1,'%d ',1)

% do first one
basename='ssep42cont_';
k=1; j=1;
confname=['eegcode.conf.' basename num2str(k) '_' num2str(j)];
ssepconf4_2([stimfreq,stimamps(j)],intparams,confname);
outname=['eegcode.output.' basename num2str(k) '_' num2str(j)];
dumpname=['eegcode.dump.' basename num2str(k) '_' num2str(j)];
system(['eeg -i ' confname ' -o ' outname ' -d ' dumpname ]);

[t,y]=readoutput2(outname);   

[P(:,j),f(:,j)]=pspec(y(:,1),deltat,discard,winlen);

if 1,
Y(:,j)=y(end-keepYn+1:end,1); % store each phie
T=t(end-keepYn+1:end);
end
% find local extrema
if 1,
    tmp=localmax(y(:,1));
    maxima(:,j)=y(tmp(end-bifpts+1:end),1);
    tmp=localmax(-y(:,1));
    minima(:,j)=y(tmp(end-bifpts+1:end),1);
end
delete(confname,outname) %%%%%%%%


for j=2:length(stimamps),
    fprintf(1,'%d ',j)
    if ~mod(j,30), fprintf(1,'\n'), end
    
    confname=['eegcode.conf.' basename num2str(k) '_' num2str(j)];
    
    eegconfrestart(dumpname,confname,stimamps(j))
    delete(dumpname) %%%%%%%% 
    
    %ssepconf4_2([stimfreqs(j),stimamp],intparams_l,confname);
    outname=['eegcode.output.' basename num2str(k) '_' num2str(j)];
    dumpname=['eegcode.dump.' basename num2str(k) '_' num2str(j)];
    system(['eeg restart -i ' confname ' -o ' outname ' -d ' dumpname ]);
    
    [t,y]=readoutput2(outname);   
    
    [P(:,j),f(:,j)]=pspec(y(:,1),deltat,discard,winlen);
    
    if 1,  % parfor takes issue with indexing P in here
    Y(:,j)=y(end-keepYn+1:end,1); % store each phie
    %figure(7)  % plots of timeseries and spectra for each stimfreq
    %subplot(6,4,2*j-1), plot(t,y(:,1)), xlim([59 60]), title(sprintf('f_D=%g',stimfreqs(j)))
    %subplot(6,4,2*j), plot(f(1:maxfi),log10(P(1:maxfi,j))), xlim([0 50])
    end
    
    % find local extrema
    if 1,
        tmp=localmax(y(:,1));
        maxima(:,j)=y(tmp(end-bifpts+1:end),1);
        tmp=localmax(-y(:,1));
        minima(:,j)=y(tmp(end-bifpts+1:end),1);
    end
    
    
    if j<length(stimamps),  % keep last one
        delete(confname,outname)
    end
end
fprintf(1,'\n')

%confname=['eegcode.conf.' basename num2str(length(stimamps))];
confname=['eegcode.conf.' basename num2str(k) '_' num2str(1)];
ssepconf4([stimfreq,stimamps(1)],intparams,confname);
f=f(:,1); % only want 1 f

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
          sprintf('noiseamp=%g  stimfreq=%g  nsteps=%d  deltat=%g  winlen=%g  discard=%g',p.noiseamp,stimfreq,nsteps,deltat,winlen,discard)};

maxfi=find(f>=50, 1 );

Psmooth=zeros(size(P));
Psmooth(2:end-1,:)=(P(2:end-1,:)+(P(1:end-2,:)+P(3:end,:))/2)/2;
Psmooth(1,:)=P(1,:); Psmooth(end,:)=P(end,:);
      
% plots
if length(stimamps)>1,
%     figure(103), pcolor(stimamps,f(1:maxfi),log(P(1:maxfi,:))), shading flat, ylim([0 f(maxfi)]), xlabel('stimamp (s^{-1})'), ylabel('Response f (Hz)')
%     title(titlestr)
%     
%     figure(104), plot(f(1:maxfi),log10(P(1:maxfi,:))), xlim([0 100])
    
    %figure(5), surf(stimfreqs,f,log(abs(Psmooth-Pref*ones(1,size(Psmooth,2))))), shading flat, view(2), ylim([0 max(stimfreqs)]), xlabel('Stimulus f (Hz)'), ylabel('Response f (Hz)')
    figure(105), pcolor(stimamps,f(1:maxfi),log10(Psmooth(1:maxfi,:))), shading flat, ylim([0 f(maxfi)]), xlabel('stimamp (s^{-1})'), ylabel('Response f (Hz)')
    title(titlestr)
    
    figure(50)
    plot(stimamps,maxima,plotstring,stimamps,minima,plotstring,'MarkerSize',5)
    xlabel('\Phi_n (s^{-1})')
    ylabel('\phi_e extrema (s^{-1})')
    title(titlestr)
end
%figure(3), surf(stimfreqs,f,log(abs(P-P(:,end)*ones(1,size(P,2))))),
%shading flat, view(2), ylim([0 100])


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
    
    % bifdiag using local minima and maxima
    figure
    plot(stimfreqs,maxima,'.',stimfreqs,minima,'.')
    xlabel('f (Hz)')
    ylabel('\phi_e extrema (s^{-1})')
    title(titlestr)
    
    % movie showing time series for each stimamp
    figure
    for j=1:length(stimamps), plot(t(end-keepYn+1:end),Y(:,j)), ylim([3 7.5]), Ymov(j)=getframe; end
    movie(Ymov)
end

% save points up to here
save bif_090716_forward stimamps maxima minima plotstring titlestr p stimfreq

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%stimfreq=17.5;
%stimamps=10:0.01:15;%10*0.5;

plotstring='k.';

clearfig=0;
if clearfig,
    figure(50), clf
end

reverse=1;
if reverse,
    stimamps=fliplr(stimamps);
    plotstring='r.';
    figure(50), hold on
end

% nsteps=400000; %500000
% deltat=0.0001;
% intparams=[nsteps,deltat];
% 
% winlen=20;  %20
% discard=20; %10
% 
% specn=floor(winlen/deltat)/2;
% keepYn=floor(10/deltat);

P=zeros(specn,length(stimamps));
f=zeros(specn,length(stimamps));  % wasteful; just to avoid temp variable
Y=zeros(keepYn,length(stimamps));  % was nsteps; now just keep winlen/2
Pref=zeros(specn,1);

% if noiseamp=0, optionally calculate local extrema for bifdiags
if 1,
    bifpts=128;
    maxima=zeros(bifpts,length(stimamps));
    minima=zeros(bifpts,length(stimamps));
end



fprintf(1,'%d total \n',length(stimamps))
fprintf(1,'%d ',1)

% do first one
basename='ssep42cont_';
k=1; j=1;
confname=['eegcode.conf.' basename num2str(k) '_' num2str(j)];
ssepconf4_2([stimfreq,stimamps(j)],intparams,confname);
outname=['eegcode.output.' basename num2str(k) '_' num2str(j)];
dumpname=['eegcode.dump.' basename num2str(k) '_' num2str(j)];
system(['eeg -i ' confname ' -o ' outname ' -d ' dumpname ]);

[t,y]=readoutput2(outname);   

[P(:,j),f(:,j)]=pspec(y(:,1),deltat,discard,winlen);

if 1,
Y(:,j)=y(end-keepYn+1:end,1); % store each phie
T=t(end-keepYn+1:end);
end
% find local extrema
if 1,
    tmp=localmax(y(:,1));
    maxima(:,j)=y(tmp(end-bifpts+1:end),1);
    tmp=localmax(-y(:,1));
    minima(:,j)=y(tmp(end-bifpts+1:end),1);
end
delete(confname,outname) %%%%%%%%


for j=2:length(stimamps),
    fprintf(1,'%d ',j)
    if ~mod(j,30), fprintf(1,'\n'), end
    
    confname=['eegcode.conf.' basename num2str(k) '_' num2str(j)];
    
    eegconfrestart(dumpname,confname,stimamps(j))
    delete(dumpname) %%%%%%%% 
    
    %ssepconf4_2([stimfreqs(j),stimamp],intparams_l,confname);
    outname=['eegcode.output.' basename num2str(k) '_' num2str(j)];
    dumpname=['eegcode.dump.' basename num2str(k) '_' num2str(j)];
    system(['eeg restart -i ' confname ' -o ' outname ' -d ' dumpname ]);
    
    [t,y]=readoutput2(outname);   
    
    [P(:,j),f(:,j)]=pspec(y(:,1),deltat,discard,winlen);
    
    if 1,  % parfor takes issue with indexing P in here
    Y(:,j)=y(end-keepYn+1:end,1); % store each phie
    %figure(7)  % plots of timeseries and spectra for each stimfreq
    %subplot(6,4,2*j-1), plot(t,y(:,1)), xlim([59 60]), title(sprintf('f_D=%g',stimfreqs(j)))
    %subplot(6,4,2*j), plot(f(1:maxfi),log10(P(1:maxfi,j))), xlim([0 50])
    end
    
    % find local extrema
    if 1,
        tmp=localmax(y(:,1));
        maxima(:,j)=y(tmp(end-bifpts+1:end),1);
        tmp=localmax(-y(:,1));
        minima(:,j)=y(tmp(end-bifpts+1:end),1);
    end
    
    
    if j<length(stimamps),  % keep last one
        delete(confname,outname)
    end
end
fprintf(1,'\n')

%confname=['eegcode.conf.' basename num2str(length(stimamps))];
confname=['eegcode.conf.' basename num2str(k) '_' num2str(1)];
ssepconf4_2([stimfreq,stimamps(1)],intparams,confname);
f=f(:,1); % only want 1 f

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
          sprintf('noiseamp=%g  stimfreq=%g  nsteps=%d  deltat=%g  winlen=%g  discard=%g',p.noiseamp,stimfreq,nsteps,deltat,winlen,discard)};

maxfi=find(f>=50, 1 );

Psmooth=zeros(size(P));
Psmooth(2:end-1,:)=(P(2:end-1,:)+(P(1:end-2,:)+P(3:end,:))/2)/2;
Psmooth(1,:)=P(1,:); Psmooth(end,:)=P(end,:);
      
% plots
if length(stimamps)>1,
%     figure(103), pcolor(stimamps,f(1:maxfi),log(P(1:maxfi,:))), shading flat, ylim([0 f(maxfi)]), xlabel('stimamp (s^{-1})'), ylabel('Response f (Hz)')
%     title(titlestr)
%     
%     figure(104), plot(f(1:maxfi),log10(P(1:maxfi,:))), xlim([0 100])
    
    %figure(5), surf(stimfreqs,f,log(abs(Psmooth-Pref*ones(1,size(Psmooth,2))))), shading flat, view(2), ylim([0 max(stimfreqs)]), xlabel('Stimulus f (Hz)'), ylabel('Response f (Hz)')
    figure(205), pcolor(stimamps,f(1:maxfi),log10(Psmooth(1:maxfi,:))), shading flat, ylim([0 f(maxfi)]), xlabel('stimamp (s^{-1})'), ylabel('Response f (Hz)')
    title(titlestr)
    
    figure(50)
    plot(stimamps,maxima,plotstring,stimamps,minima,plotstring,'MarkerSize',5)
    xlabel('\Phi_n (s^{-1})')
    ylabel('\phi_e extrema (s^{-1})')
    title(titlestr)
end
%figure(3), surf(stimfreqs,f,log(abs(P-P(:,end)*ones(1,size(P,2))))),
%shading flat, view(2), ylim([0 100])

save bif_090716_back stimamps maxima minima plotstring titlestr p stimfreq

% save figure
figure(50), hgsave('bif_090716')




