function [f_arr,P_arr,t,y] = run_neurofield(p,file_id)
    % This function executes neurofield on a standard point struct
    % Only one set of nuab and phia are allowed
    % Make sure that 'neurofield' is on your path
    if nargin < 2
        file_id = 1;
    end
    
    lin = [0 0 0]; % [cortex reticular relay] for linear functions
    linfun(1) = p.phia(1).*(1-p.phia(1)/p.qmax)/p.sigma;
    linfun(2) = p.phia(1)-linfun(1)*sinv(p.phia(1),p);
    linfun(3) = p.phia(2).*(1-p.phia(2)/p.qmax)/p.sigma;
    linfun(4) = p.phia(2)-linfun(3)*sinv(p.phia(2),p);
    linfun(5) = p.phia(3).*(1-p.phia(3)/p.qmax)/p.sigma;
    linfun(6) = p.phia(3)-linfun(5)*sinv(p.phia(3),p);
    
    grid_output = 0; % grid_output 1 = work with the entire array of nodes
    grid_size = 3; % number of nodes per edge...total number is squared
	if size(p.nus,1) ~= 1 || size(p.phia,1) ~= 1
		error('Input point must have exactly one set of nuab and phia')
	end

	fid = fopen(sprintf('neurofield_%i.conf',file_id),'w');

    % integration data
    sampling_rate = 10000; % Sampling rate (Hz)
    deltat = 1/sampling_rate;
    int_time = 30; % Total integration time in seconds
    nsteps = int_time/deltat;
    
	deltax= 0.0035;
	
	% CFL condition
	v = p.gammae*p.re; % Axonal velocity
	if deltat >= deltax/v % If the timestep is larger than the time required to propagate deltax
		fprintf(2,'Modifying deltat to satisfy CFL stability condition\n')
		deltat = deltax/v * 0.9;
		fprintf(1,'Deltat = %f\n',deltat);
	end
	
	% Calculate the keyring size
	dsize = round(p.taues/deltat);
    
    noiseamp=1; % Noise amplitude
    %noiseamp = sqrt(p.phin^2/deltat/2); % Correctly set the noise amplitude for input point
       
    fprintf(fid,'A Configuration file for the code ''NeuroField''\n');
    fprintf(fid,'Nodes per population: %i\n',grid_size^2);
    fprintf(fid,'Number of neural populations: 5\n');
    fprintf(fid,'Number of neural connections: 11\n');
    fprintf(fid,'\n');
    fprintf(fid,'Population connection matrix  1   2   3   4   5\n');
    fprintf(fid,'1	                          :1  :5  :0  :8  :0\n');
    fprintf(fid,'2			                  :2  :6  :0  :9  :0\n');
    fprintf(fid,'3			                  :3  :0  :0  :10 :0\n');
    fprintf(fid,'4			                  :4  :0  :7  :0  :11\n');
    fprintf(fid,'5			                  :0  :0  :0  :0  :0 \n');
    fprintf(fid,'Number of integration steps: %d Deltat: %f \n',nsteps,deltat);
    fprintf(fid,'Q delay depths: %d : 0 : 0 : %d :0\n',dsize+15,dsize+15);
    fprintf(fid,'Propagator types 1: Waveeqn 2: Waveeqn 3: Waveeqn 4: Waveeqn 5: Mapping $\n');
    fprintf(fid,'6: Mapping 7: Mapping 8: Mapping 9: Mapping 10: Mapping 11: Mapping $\n');
    fprintf(fid,'\n');
    fprintf(fid,'Coupling types 1: Simple 2: Simple 3: Simple 4: Simple 5: Simple 6: Simple $\n');
    fprintf(fid,'7: Simple 8: Simple 9: Simple 10: Simple 11: Simple $\n');
    fprintf(fid,'\n');
    fprintf(fid,'Lambda: 0 tGlu: 0\n');
    fprintf(fid,'Population 1 - Excitory neurons\n');
    fprintf(fid,' Initial Q: %f\n',p.phia(1));
    if lin(1)
        fprintf(fid,'  Firing response Linear: Gradient: %f Intercept: %f\n',linfun(1),linfun(2));
    else
        fprintf(fid,'  Firing response Theta: %f Sigma: %untitled.jpgf Qmax: %f\n',p.theta,p.sigma,p.qmax);
    end
    fprintf(fid,' Number of Dendritic responses: 3\n');
    fprintf(fid,'  Dendritic response from population 1 V initial: Steady alpha: %f beta: %f\n',p.alpha_ab(1),p.beta_ab(1));  % Initial V is actually Vab
    fprintf(fid,'  Dendritic response from population 2 V initial: Steady alpha: %f beta: %f\n',p.alpha_ab(2),p.beta_ab(2));
    fprintf(fid,'  Dendritic response from population 4 V initial: Steady alpha: %f beta: %f\n',p.alpha_ab(3),p.beta_ab(3));
    fprintf(fid,'\n');
    fprintf(fid,'Population 2 - Inhibitory neurons\n');
    fprintf(fid,'  Initial Q: %f\n',p.phia(1));
    if lin(1)
        fprintf(fid,'  Firing response Linear: Gradient: %f Intercept: %f\n',linfun(1),linfun(2));
    else
        fprintf(fid,'  Firing response Theta: %f Sigma: %f Qmax: %f\n',p.theta,p.sigma,p.qmax);
    end
    fprintf(fid,'  Number of Dendritic responses: 3\n');
    fprintf(fid,'   Dendritic response from population 1 V initial: Steady alpha: %f beta: %f\n',p.alpha_ab(1),p.beta_ab(1));
    fprintf(fid,'   Dendritic response from population 2 V initial: Steady alpha: %f beta: %f\n',p.alpha_ab(2),p.beta_ab(2));
    fprintf(fid,'   Dendritic response from population 4 V initial: Steady alpha: %f beta: %f\n',p.alpha_ab(3),p.beta_ab(3));
    fprintf(fid,'\n');
    fprintf(fid,'Population 3 - Reticular nucleus neurons\n');
    fprintf(fid,'  Initial Q: %f\n',p.phia(2));
    if lin(2)
        fprintf(fid,'  Firing response Linear: Gradient: %f Intercept: %f\n',linfun(3),linfun(4));
    else
        fprintf(fid,'  Firing response Theta: %f Sigma: %f Qmax: %f\n',p.theta,p.sigma,p.qmax);
    end
    fprintf(fid,'  Number of Dendritic responses: 2\n');
    fprintf(fid,'   Dendritic response from population 1 V initial: Steady alpha: %f beta: %f\n',p.alpha_ab(7),p.beta_ab(7));
    fprintf(fid,'   Dendritic response from population 4 V initial: Steady alpha: %f beta: %f\n',p.alpha_ab(8),p.beta_ab(8));
    fprintf(fid,'\n');
    fprintf(fid,'Population 4 - Relay nuclei neurons\n');
    fprintf(fid,'  Initial Q: %f\n',p.phia(3));
    if lin(3)
        fprintf(fid,'  Firing response Linear: Gradient: %f Intercept: %f\n',linfun(5),linfun(6));
    else
        fprintf(fid,'  Firing response Theta: %f Sigma: %f Qmax: %f\n',p.theta,p.sigma,p.qmax);
    end
    fprintf(fid,'  Number of Dendritic responses: 3\n');
    fprintf(fid,'   Dendritic response from population 1 V initial: Steady alpha: %f beta: %f\n',p.alpha_ab(4),p.beta_ab(4));
    fprintf(fid,'   Dendritic response from population 3 V initial: Steady alpha: %f beta: %f\n',p.alpha_ab(5),p.beta_ab(5));
    fprintf(fid,'   Dendritic response from population 5 V initial: Steady alpha: %f beta: %f\n',p.alpha_ab(6),p.beta_ab(6)); % changed from phi_n(0)==0
    fprintf(fid,'\n');
    fprintf(fid,'Population 5 - Stimulus neurons\n');
    fprintf(fid,'  Initial Q: %f\n',1);  % changed from 0
    fprintf(fid,'  Stimulus mode: 2 Time to start of stimulus: 0.0 Amplitude: %f Mean: 1\n',noiseamp);
	fprintf(fid,'\n');
    fprintf(fid,'Propagation data\n');
    fprintf(fid,'Propagator 1  - Initial Phi: Steady Deltax: %f Tauabt: 0   Effective range: %f gamma: %f\n',deltax,p.re,p.gammae);
    fprintf(fid,'Propagator 2  - Initial Phi: Steady Deltax: %f Tauabt: 0   Effective range: %f gamma: %f\n',deltax,p.re,p.gammae);
    fprintf(fid,'Propagator 3  - Initial Phi: Steady Deltax: %f Tauabt: %d Effective range: %f gamma: %f\n',deltax,p.tause,p.re,p.gammae);
    fprintf(fid,'Propagator 4  - Initial Phi: Steady Deltax: %f Tauabt: %d Effective range: %f gamma: %f\n',deltax,p.tause,p.re,p.gammae);
    fprintf(fid,'Propagator 5  - Tauab: 0   \n');
    fprintf(fid,'Propagator 6  - Tauab: 0   \n');
    fprintf(fid,'Propagator 7  - Tauab: 0   \n');
    fprintf(fid,'Propagator 8  - Tauabt: %d \n',p.taues);  % why wave eqn for phis here?
    fprintf(fid,'Propagator 9  - Tauabt: %d \n',p.taues);  %  ==> changing to mapping
    fprintf(fid,'Propagator 10 - Tauab: 0   \n');
    fprintf(fid,'Propagator 11 - Tauab: 0   \n');
    fprintf(fid,'Coupling data 1  - Nu: %f\n',p.nus(1));
    fprintf(fid,'Coupling data 2  - Nu: %f\n',p.nus(1));
    fprintf(fid,'Coupling data 3  - Nu: %f\n',p.nus(7));
    fprintf(fid,'Coupling data 4  - Nu: %f\n',p.nus(4));
    fprintf(fid,'Coupling data 5  - Nu: %f\n',p.nus(2));
    fprintf(fid,'Coupling data 6  - Nu: %f\n',p.nus(2));
    fprintf(fid,'Coupling data 7  - Nu: %f\n',p.nus(5));
    fprintf(fid,'Coupling data 8  - Nu: %f\n',p.nus(3));
    fprintf(fid,'Coupling data 9  - Nu: %f\n',p.nus(3));
    fprintf(fid,'Coupling data 10 - Nu: %f\n',p.nus(8));
    fprintf(fid,'Coupling data 11 - Nu: %f\n',p.nus(6));
    fprintf(fid,'\n');
    if grid_output
        fprintf(fid,'Output Data - Number of traces : %i \n',grid_size^2);
        fprintf(fid,'Wave Equation Number :1 Single/All: All nodes\n'); % phie
    else
        fprintf(fid,'Output Data - Number of traces : 5 \n');
        fprintf(fid,'Wave Equation Number :1 Single/All: Single node number :1\n'); % phie
        fprintf(fid,'Wave Equation Number :5 Single/All: Single node number :1\n'); % phii
        fprintf(fid,'Wave Equation Number :7 Single/All: Single node number :1\n'); % phir
        fprintf(fid,'Wave Equation Number :10 Single/All: Single node number :1\n'); % phis - the phis going into Vr
        fprintf(fid,'Wave Equation Number :11 Single/All: Single node number :1\n'); % phin
    end
    
    fclose(fid);

    fprintf(1,'Executing NF...');
    tic;
	[status] = system(sprintf('/home/xzhao/neurofield/trunk/Release/NeuroField -i neurofield_%i.conf -d neurofield_%i.dump -o neurofield_%i.output',file_id,file_id,file_id));
    fprintf(1,'took %.3f seconds\n',toc);
    
    if status ~= 0
        error('An error occurred when running Neurofield');
        return
    end
    	
	% Code copied directly from readoutput2() (James)
	fid=fopen(sprintf('neurofield_%i.output',file_id));x

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
	temp=fgetl(fid);
	while length(temp)>20,
	    temp=fgetl(fid);
	end
	y=fscanf(fid,'%f');
	y=reshape([sscanf(temp,'%f');y],ntraces,nsteps/(skippoints+1))';
	t=[0:deltat*(skippoints+1):(nsteps-1)*deltat]';

	fclose(fid);
	
	% Plot the time series
	figure
	pos = get(gcf,'Position');
	
	subplot(1,2,1);
	plot(t,y);
	legend('Excitatory','Inhibitory','Reticular','Relay')
    title('Neurofield time series')
    
    % Get the power spectrum
    decimation_factor = 1;
    v = decimate(y(:,1)-mean(y(:,1)),decimation_factor);
    [f{1},P{1}] = pwelch_spectrum(v,1/deltat/decimation_factor);


    f_arr = f{1};
    P_arr = P{1}; % Use pwelch as output

    
    % Plot the power spectrum
	subplot(1,2,2);
    loglog(f{1},P{1});

    set(gca,'XLim',[1 45]);
    
    legend('neurofield','Analytic');
    xlabel('Frequency (Hz)');
    ylabel('Power (arbitrary)');
    title(sprintf('X: %.3f Y: %.3f Z: %.3f',p.xyz(1),p.xyz(2),p.xyz(3)));
    set(gcf,'Position',pos+[-1 0 1 0].*pos);
end

