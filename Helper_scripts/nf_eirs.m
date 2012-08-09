function varargout = nf_eirs(p,file_id,nonlinear,int_time,grid_edge,grid_output,fs)
    % Run NeuroField on an EIRS point struct
    % [nf,f,P] = nf_eirs(p,file_id,nonlinear,int_time,grid_edge,grid_output)
    % - Accepts a point struct as input
    % - file_id optionally specifies the number for neurofield_*.conf/output
    % - nonlinear is a 3 element vector that equals 1 to enable nonlinearity
    %   e.g. [0 0 1] will only use a sigmoid population response in relay nuclei
    %   System is fully nonlinear by default 
    % - grid_edge: specify edge size of the grid (in number of nodes). Center node is automatically outputted
    % - grid_output: outputs grid data for just the excitatory neurons, and returns
    %                spectrum with spectral filtering
    %   This program writes the conf file, executes it, and parses the output
    %
    % WARNING- THIS FUNCTION ASSUMES SAME ALPHA AND BETA FOR ALL CONNECTIONS
    % WARNING- THIS FUNCTION ASSUMES 'neurofield' IS ON THE SHELL PATH
    % WARNING- Depends on nf_read, nf_extract, pwelch_spectrum, and optionally 
    
    if nargin < 7 || isempty(fs)
        fs = 10000;
    end
    
    if nargin < 6 || isempty(grid_output)
        grid_output = 0;
    end
    
    if nargin < 5 || isempty(grid_edge)
        grid_edge = 12;
    end

    if nargin < 4 || isempty(int_time)
        int_time = 30; % Total integration time (s)
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
    deltat = 1/fs;
    Lx = 0.5; % Cortex linear dimension (metres)
	deltax= Lx/grid_edge; % Assume square grid
	%deltax = 0.0035;
	
	% CFL condition
	v = p.gammae*p.re; % Axonal velocity
	if deltat >= deltax/v % If the timestep is larger than the time required to propagate deltax
		fprintf(2,'Modifying deltat to satisfy CFL stability condition\n')
		deltat = deltax/v * 0.9;
		fprintf(1,'Deltat = %f\n',deltat);
	end
	
    
    %noiseamp = noise_multiplier*sqrt(p.phin^2/deltat/2)
    %noiseamp = 0.7071
    %noiseamp = noise_multiplier*sqrt(grid_edge*grid_edge*p.phin^2/deltat/2/2/pi/2/pz
    %deltat
    %deltax
    noiseamp = 0.01*sqrt(p.phin^2/deltat/deltax/deltax*4*pi^2)/2/pi;
    noiseamp = sqrt(p.phin^2/deltat/deltax/deltax*4*pi^2/2)
    %return
    
    % WRITE THE FILE
    fid = fopen(sprintf('neurofield_%i.conf',file_id),'w');
    
    fprintf(fid,'EIRS model, automatically generated with nf_eirs.m\n');
    fprintf(fid,'Time: %f Deltat: %f\n',int_time,deltat);
    fprintf(fid,'Nodes: %i\n',grid_edge^2);
    fprintf(fid,'Glutamate dynamics - fast Lambda: 0 fast Glu: 0\n');
    fprintf(fid,'                     slow Lambda: 0 slow Glu: 0\n');
                     

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
    
    deltax = 0.1*deltax;
    fprintf(fid,'Propag 1: Wave - Tau: %f Deltax: %f Range: %f gamma: %f\n',0,deltax,p.re,p.gammae);
    fprintf(fid,'Propag 2: Map - Tau: %f\n',0);   
    fprintf(fid,'Propag 3: Map - Tau: %f\n',p.taues);   
    fprintf(fid,'Propag 4: Wave - Tau: %f Deltax: %f Range: %f gamma: %f\n',0,deltax,p.re,p.gammae);
    fprintf(fid,'Propag 5:  Map - Tau: %f\n',0);  
    fprintf(fid,'Propag 6:  Map - Tau: %f\n',p.tause); 
    fprintf(fid,'Propag 7: Wave - Tau: %f Deltax: %f Range: %f gamma: %f\n',p.tause,deltax,p.re,p.gammae);
    fprintf(fid,'Propag 8:  Map - Tau: %f\n',0);  
    fprintf(fid,'Propag 9: Wave - Tau: %f Deltax: %f Range: %f gamma: %f\n',p.tause,deltax,p.re,p.gammae);
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

    if grid_output
        %fprintf(fid,'Output: Node: All Start: 0 Interval: .004\n');
        fprintf(fid,'Output: Node: All Start: 0\n');
        fprintf(fid,'Population:\n');
        fprintf(fid,'Propag: 1\n');
        fprintf(fid,'Couple:\n');
    else
        fprintf(fid,'Output: Node: %d\n',round((grid_edge^2 + grid_edge)/2));
        fprintf(fid,'Population:\n');
        fprintf(fid,'Propag: 1 2 10 8\n');
        fprintf(fid,'Couple:\n');
    end
    
    fclose(fid);
    
    if grid_output
        fprintf(1,'Integration time: %d s sampled at %d Hz\nGrid size %dx%d, outputting all nodes\nSimulating...',int_time,1/deltat,grid_edge,grid_edge);
    else
        fprintf(1,'Integration time: %d s sampled at %d Hz\nGrid size %dx%d, outputting node %d\nSimulating...',int_time,1/deltat,grid_edge,grid_edge,round((grid_edge^2 + grid_edge)/2));
    end
    
    nf = nf_run(sprintf('neurofield_%i',file_id));

    if nargout > 0
        varargout{1} = nf;
    end
    if nargout > 1
        varargout{2} = f;
    end
    if nargout > 2
        varargout{3} = P;
    end
    
    return
    
    if grid_output == 0 % If outputting only one node, include the time series
        data = nf_extract(nf,{'propag.1.phi','propag.2.phi','propag.10.phi','propag.8.phi'});
	    % Plot the time series
	    figure
	    subplot(1,2,1);
	    plot(nf.time,data);
	    legend('Excitatory','Inhibitory','Reticular','Relay')
        title('Neurofield time series')
        xlabel('Time (s)');
        ylabel('Phi (s^{-1})');
    
        % Get the power spectrum from a single node
        decimation_factor = 1;
        v = decimate(detrend(data(:,1)),decimation_factor);
        [f,P] = pwelch_spectrum(v,1/nf.deltat/decimation_factor);
        subplot(1,2,2);
        loglog(f,P,'b');
        xlabel('Frequency (Hz)');
        ylabel('Power (arbitrary)');
        title(sprintf('X: %.3f Y: %.3f Z: %.3f',p.xyz(1),p.xyz(2),p.xyz(3)));
        set(gca,'XLim',[1 45]);
    else
        figure
        [f,Pnew,Pold] = nf_spatial_filter(nf,p);
        loglog(f,Pnew,'b');
        xlabel('Frequency (Hz)');
        ylabel('Power (arbitrary)');
        title(sprintf('X: %.3f Y: %.3f Z: %.3f',p.xyz(1),p.xyz(2),p.xyz(3)));
        set(gca,'XLim',[1 45]);
        %hold on
        %loglog(f,Pold,'g');
        %hold off
    end
    
    try
        [f_a,P_a] = analytic_spectrum(p,1);
        P_a = interp1(f_a,P_a,f,'pchip','extrap');
        hold on
        loglog(f,P_a,'r--');
        if grid_output
            legend('Spatially filtered','Analytic');
        else
            legend('neurofield','Analytic');
        end
        hold off
    end
    
    if grid_output == 0
        set(gcf,'Position',get(gcf,'Position')+[-1 0 1 0].*get(gcf,'Position')); % Double the horizontal size of the figure
    end

