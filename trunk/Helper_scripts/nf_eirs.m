function varargout = nf_eirs(p,file_id,nonlinear,grid_edge)
    % Run NeuroField on an EIRS point struct
    % [nf,f,P] = nf_eirs(p,file_id,linearize,grid_size)
    % - Accepts a point struct as input
    % - file_id optionally specifies the number for neurofield_*.conf/output
    % - nonlinear is a 3 element vector that equals 1 to enable nonlinearity
    %   e.g. [0 0 1] will only use a sigmoid population response in relay nuclei
    %   System is fully nonlinear by default 
    % - grid_edge: specify edge size of the grid (in number of nodes). Center node is automatically outputted
    %   This program writes the conf file, executes it, and parses the output
    %
    % WARNING- THIS FUNCTION ASSUMES SAME ALPHA AND BETA FOR ALL CONNECTIONS
    % WARNING- THIS FUNCTION ASSUMES 'neurofield' IS ON THE SHELL PATH
    % WARNING- Depends on nf_read, nf_extract, pwelch_spectrum, and optionally 
    
    if nargin < 4 || isempty(grid_edge)
        grid_edge = 3;
    end

    if nargin < 3 || isempty(nonlinear)
        nonlinear = [1 1 1 1];
    else
        nonlinear = [nonlinear(1) nonlinear(1) nonlinear(2:3)]; % Add an entry for inhibitory
    end
    
    if nargin < 2 || isempty(file_id)
        file_id = 1;
    end
    
    if size(p.nus,1) ~= 1 || size(p.phia,1) ~= 1
		error('Input point must have exactly one set of nuab and phia')
	end
	
	% Initialize numerical solver parameters
    deltat = 1e-4;
    int_time = 30; % Total integration time (s)
	deltax= 0.0035;
	% CFL condition
	v = p.gammae*p.re; % Axonal velocity
	if deltat >= deltax/v % If the timestep is larger than the time required to propagate deltax
		fprintf(2,'Modifying deltat to satisfy CFL stability condition\n')
		deltat = deltax/v * 0.9;
		fprintf(1,'Deltat = %f\n',deltat);
	end
	
    noiseamp = sqrt(p.phin^2/deltat/2);
    
    % WRITE THE FILE
    fid = fopen(sprintf('neurofield_%i.conf',file_id),'w');
    
    fprintf(fid,'EIRS model, automatically generated with nf_eirs.m\n');
    fprintf(fid,'Time: %f Deltat: %f\n',int_time,deltat);
    fprintf(fid,'Nodes: %i\n',grid_edge^2);
    fprintf(fid,'Glutamate dynamics - Lambda: 0 tGlu: 0\n');
    fprintf(fid,'\n');
            
    fprintf(fid,'    Connection matrix:\n');
    fprintf(fid,'From:  1  2  3  4  5\n');
    fprintf(fid,'To 1:  1  2  0  3  0\n');
    fprintf(fid,'To 2:  4  5  0  6  0\n');
    fprintf(fid,'To 3:  7  0  0  8  0\n');
    fprintf(fid,'To 4:  9  0  10 0  11\n');
    fprintf(fid,'To 5:  0  0  0  0  0 \n\n');

    labels = {'Excitatory','Inhibitory','Reticular','Relay'};
    phivals = [p.phia(1) p.phia(1) p.phia(2) p.phia(3)];
    n_dendrites = [3 3 2 3]; % Number of dendrites on each population
    counter = 1;
    for j = 1:4 % For each population
        fprintf(fid,'Population %d: %s\n',j,labels{j});
        fprintf(fid,'Q: %f\n',phivals(j));
        if nonlinear(j) == 0 % If population has been linearized
        	gradient = phivals(j).*(1-phivals(j)/p.qmax)/p.sigma;
            intercept = phivals(j)-gradient*sinv(phivals(j),p);
            fprintf(fid,'Firing: Linear - Gradient: %f Intercept: %f\n',gradient,intercept);
        else
            fprintf(fid,'Firing: Sigmoid - Theta: %f Sigma: %f Qmax: %f\n',p.theta,p.sigma,p.qmax);
        end
        for k = 1:n_dendrites(j)
            fprintf(fid,'Dendrite %d: alpha: %f beta: %f\n',counter,p.alpha,p.beta);
            counter = counter + 1;
        end
        fprintf(fid,'\n');
    end

    fprintf(fid,'Population 5: Stimulation\n');
    fprintf(fid,'Stimulus: Mode: White - Onset: 0 Amplitude: %f Mean: 1\n',noiseamp);
    fprintf(fid,'\n');
    
    fprintf(fid,'Propag 1: Wave - Deltax: %f Tau: %f Range: %f gamma: %f\n',deltax,0,p.re,p.gammae);
    fprintf(fid,'Propag 2: Map - Tau: %f\n',0);   
    fprintf(fid,'Propag 3: Map - Tau: %f\n',p.taues);   
    fprintf(fid,'Propag 4: Wave - Deltax: %f Tau: %f Range: %f gamma: %f\n',deltax,0,p.re,p.gammae);
    fprintf(fid,'Propag 5:  Map - Tau: %f\n',0);  
    fprintf(fid,'Propag 6:  Map - Tau: %f\n',p.tause); 
    fprintf(fid,'Propag 7: Wave - Deltax: %f Tau: %f Range: %f gamma: %f\n',deltax,p.tause,p.re,p.gammae);
    fprintf(fid,'Propag 8:  Map - Tau: %f\n',0);  
    fprintf(fid,'Propag 9: Wave - Deltax: %f Tau: %f Range: %f gamma: %f\n',deltax,p.tause,p.re,p.gammae);
    fprintf(fid,'Propag 10: Map - Tau: %f\n',0);  
    fprintf(fid,'Propag 11: Map - Tau: %f\n',0);   
    fprintf(fid,'Couple 1:  Map - nu: %f\n',p.nus(1)); % ee
    fprintf(fid,'Couple 2:  Map - nu: %f\n',p.nus(2)); % ei
    fprintf(fid,'Couple 3:  Map - nu: %f\n',p.nus(3)); % es
    fprintf(fid,'Couple 4:  Map - nu: %f\n',p.nus(1)); % ie = ee
    fprintf(fid,'Couple 5:  Map - nu: %f\n',p.nus(2)); % ii = ei
    fprintf(fid,'Couple 6:  Map - nu: %f\n',p.nus(3)); % is = es
    fprintf(fid,'Couple 7:  Map - nu: %f\n',p.nus(7)); % re
    fprintf(fid,'Couple 8:  Map - nu: %f\n',p.nus(8)); % rs
    fprintf(fid,'Couple 9:  Map - nu: %f\n',p.nus(4)); % se
    fprintf(fid,'Couple 10: Map - nu: %f\n',p.nus(5)); % sr
    fprintf(fid,'Couple 11: Map - nu: %f\n',p.nus(6)); % sn
    fprintf(fid,'\n');

    fprintf(fid,'Output: Node: %d\n',round((grid_edge^2 + grid_edge)/2));
    fprintf(fid,'Population:\n');
    fprintf(fid,'Propag: 1 2 10 8\n');
    fprintf(fid,'Couple:\n');

    fclose(fid);
    
    fprintf(1,'Integration time: %d s sampled at %d Hz\nGrid size %dx%d, outputting node %d\nSimulating...',int_time,1/deltat,grid_edge,grid_edge,round((grid_edge^2 + grid_edge)/2));
    tic;
    [status] = system(sprintf('neurofield -i neurofield_%i.conf -d neurofield_%i.dump -o neurofield_%i.output',file_id,file_id,file_id));
    fprintf(1,'took %.3f seconds\n',toc);
    
    if status ~= 0
        error('An error occurred when running Neurofield');
        return
    end
    
    fprintf(1,'Parsing output...');
    nf = nf_read(sprintf('neurofield_%i.output',file_id));
    data = nf_extract(nf,{'propag.1.phi','propag.2.phi','propag.10.phi','propag.8.phi'});
    fprintf(1,'done!\n');
    
	% Plot the time series
	figure
	subplot(1,2,1);
	plot(nf.time,data);
	legend('Excitatory','Inhibitory','Reticular','Relay')
    title('Neurofield time series')
    xlabel('Time (s)');
    ylabel('Phi (s^{-1})');
    
    % Get the power spectrum
    decimation_factor = 1;
    v = decimate(detrend(data(:,1)),decimation_factor);
    [f,P] = pwelch_spectrum(v,1/nf.deltat/decimation_factor);
    subplot(1,2,2);
    loglog(f,P,'b');
    xlabel('Frequency (Hz)');
    ylabel('Power (arbitrary)');
    title(sprintf('X: %.3f Y: %.3f Z: %.3f',p.xyz(1),p.xyz(2),p.xyz(3)));
    set(gca,'XLim',[1 45]);
    
    try
        [f_a,P_a] = analytic_spectrum(p,1);
        % And renormalize it
        P_a = interp1(f_a,P_a,f,'pchip','extrap');
        P_a = P_a * (P_a\P);
        hold on
        loglog(f,P_a,'r--');
        legend('neurofield','Analytic');
        hold off
    end
    

    set(gcf,'Position',get(gcf,'Position')+[-1 0 1 0].*get(gcf,'Position')); % Double the horizontal size of the figure
        
    if nargout > 0
        varargout{1} = nf;
        varargout{2} = f;
        varargout{3} = P;
    end

