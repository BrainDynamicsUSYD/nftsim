function varargout = nf_eirs(p,file_id,firemode,int_time,grid_edge,fs,waves)
    % Run NeuroField on an EIRS point struct
    % [nf,f,P,V] = nf_eirs(p,file_id,firemode,int_time,grid_edge,grid_output)
    % - Accepts a point struct as input
    % - file_id optionally specifies the number for neurofield_*.conf/output
    % - firemode is a 3 element vector that equals
    %   - 0 for fully nonlinear
    %   - 1 for linear
    %   - 2 for quadratic
    %   e.g. [0 0 1] will only use a sigmoid population response in relay nuclei
    %   System is fully nonlinear by default 
    % - grid_edge: specify edge size of the grid (in number of nodes). Center node is automatically outputted
    % - grid_output: outputs grid data for just the excitatory neurons, and returns
    %                spectrum with spectral filtering
    %   This program writes the conf file, executes it, and parses the output
    %
    % WARNING- THIS FUNCTION ASSUMES SAME ALPHA AND BETA FOR ALL CONNECTIONS
    % WARNING- THIS FUNCTION ASSUMES 'neurofield' IS ON THE SHELL PATH
    % WARNING- Depends on nf_read, nf_extract, and nf_grid 
    %
    % Defaults are- 20x20 grid for 0.5m x 0.5m cortex
    %               for 30s at 10000Hz with nonlinearity
    % 
    % PLOTTING BEHAVIOUR
    % If no output arguments are specified >>nf_eirs() then nothing will be
    % produced. The end result is to produce a NeuroField output file
    % If nf = nf_eirs() then nf_read will be called but no plot produced
    % If [nf,f,P] = nf_eirs() then nf_spatial_spectrum will also be called
    % and a plot produced
    if nargin < 7 || isempty(waves)
        waves = 0; % waves = 1 for wave propagators in stimulus and relay nuclei
    end
    
    if nargin < 6 || isempty(fs)
        fs = 10000;
    end
    
    if nargin < 5 || isempty(grid_edge)
        grid_edge = 12;
    else
        if mod(grid_edge,2)
            error('The number of grid edge nodes should be even to correctly align the FFT components')
        end
    end

    if nargin < 4 || isempty(int_time)
        int_time = 15; % Total integration time (s)
    end
    
    if nargin < 3 || isempty(firemode)
        firemode = [0 0 0 0];
    else
        firemode = [firemode(1) firemode(1) firemode(2:3)]; % Add an entry for inhibitory
    end
    
    if nargin < 2 || isempty(file_id)
        file_id = 1;
    end
    
    if size(p.nus,1) ~= 1 || size(p.phia,1) ~= 1
        error('Input point must have exactly one set of nuab and phia')
    end
	
	% Initialize numerical solver parameters
    deltat = 1/fs;
    Lx = 0.5; % Cortex linear dimension (meters)
	deltax= Lx/grid_edge; % Assume square grid
	
	% CFL condition
	v = p.gammae*p.re; % Axonal velocity
    if deltat >= deltax/v % If the timestep is larger than the time required to propagate deltax
        fprintf(2,'Modifying deltat to satisfy CFL stability condition\n');
        deltat = deltax/v * 0.9;
        fprintf(1,'Deltat = %f\n',deltat);
    end
    
    % Write the file
    write_nf(file_id,p,int_time,deltat,deltax,grid_edge,firemode,waves);

    if nargout > 0
        varargout{1} = nf_run(sprintf('neurofield_%i',file_id));
    else
        nf_run(sprintf('neurofield_%i',file_id));
    end
    
    if nargout > 3
        [f,P,V] = nf_spatial_spectrum(varargout{1},'propag.1.phi',999,8,1); % kmax=4, 8 segments, spatial filtering=1
        varargout{4} = V;
    elseif nargout > 1
        [f,P] = nf_spatial_spectrum(varargout{1},'propag.1.phi',999,8,1); % kmax=4, 8 segments, spatial filtering=1
    end
    
    if nargout > 1    
        varargout{2} = f;
        varargout{3} = P;
        figure
        loglog(f,P,'b');
        xlabel('Frequency (Hz)');
        ylabel('Power (arbitrary)');
        try
            title(sprintf('X: %.3f Y: %.3f Z: %.3f',p.xyz(1),p.xyz(2),p.xyz(3)));
        end
        set(gca,'XLim',[1 45]);

        try
            [f_a,P_a] = analytic_spectrum(p,any(firemode==0));
            P_a = interp1(f_a,P_a,f,'pchip','extrap');
            hold on
            loglog(f,P_a,'r--');
            legend('NeuroField','Analytic');
            hold off
        catch
            kdb
        end
    end
end

function write_nf(file_id,p,int_time,deltat,deltax,grid_edge,firemode,waves)
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
        if firemode(j) == 1 % If population has been linearized
            v0 = sinv(phivals(j),p);
            a = rho1(phivals(j),p);
            b = phivals(j) - a*v0;
            fprintf(fid,'Firing: Linear - a: %f b: %f\n',a,b);
            fprintf(1,'Linear %s\n',labels{j});
        elseif firemode(j) == 2 % If population has been quadraticized
            v0 = sinv(phivals(j),p);
            a = rho2(phivals(j),p)/2;
            b = rho1(phivals(j),p) - 2*a*v0;
            c = phivals(j) - rho1(phivals(j),p)*v0 + a*v0.^2;
            fprintf(fid,'Firing: Quadratic - a: %f b: %f c: %f\n',a,b,c);
            fprintf(1,'Quadratic %s\n',labels{j});
        elseif firemode(j) == 3 % Cubic population
            v0 = sinv(phivals(j),p);
            rho3 = @(Q,p) 6*Q^2*rho1(Q,p)/p.sigma^2/p.qmax^2 - 6*Q*rho1(Q,p)/p.sigma^2/p.qmax + rho1(Q,p)/p.sigma^2;
            r3 = rho3(phivals(j),p);
            r2 = rho2(phivals(j),p);
            r1 = rho1(phivals(j),p);
            d =  - (r3*v0^3)/6 + (r2*v0^2)/2 - r1*v0 + phivals(j);
            c =  (r3*v0^2)/2 - r2*v0 + r1;
            b = r2/2 - (r3*v0)/2;
            a = r3/6;
            fprintf(fid,'Firing: Cubic - a: %f b: %f c: %f d: %f\n',a,b,c,d);
            fprintf(1,'Cubic %s\n',labels{j});
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
    fprintf(fid,'Stimulus: WhiteFourier - Onset: 0 phin: %f steady: 1 Deltax: %f\n',p.phin,deltax);
    
    fprintf(fid,'\n');
    if waves
        fprintf(1,'Wave propagators in stimulus and relay\n');
        fprintf(fid,'Propag 1: Wave - Tau: %f Deltax: %f Range: %f gamma: %f\n',0,deltax,p.re,p.gammae); %e CX
        fprintf(fid,'Propag 2: Map - Tau: %f\n',0);   %i CX
        fprintf(fid,'Propag 3: Wave - Tau: %f Deltax: %f Range: %f gamma: %f\n',p.taues,deltax,p.rs,p.gammas); % relay
        fprintf(fid,'Propag 4: Wave - Tau: %f Deltax: %f Range: %f gamma: %f\n',0,deltax,p.re,p.gammae);%e CX
        fprintf(fid,'Propag 5:  Map - Tau: %f\n',0); %i CX 
        fprintf(fid,'Propag 6: Wave - Tau: %f Deltax: %f Range: %f gamma: %f\n',p.taues,deltax,p.rs,p.gammas); % relay
        fprintf(fid,'Propag 7: Wave - Tau: %f Deltax: %f Range: %f gamma: %f\n',p.tause,deltax,p.re,p.gammae);%e CX
        fprintf(fid,'Propag 8: Wave - Tau: %f Deltax: %f Range: %f gamma: %f\n',0,deltax,p.rs,p.gammas); % relay
        fprintf(fid,'Propag 9: Wave - Tau: %f Deltax: %f Range: %f gamma: %f\n',p.tause,deltax,p.re,p.gammae);%e CX
        fprintf(fid,'Propag 10: Map - Tau: %f\n',0);  % nRT
        fprintf(fid,'Propag 11: Wave - Tau: %f Deltax: %f Range: %f gamma: %f\n',0,deltax,p.rn,p.gamman);   % STIM
    else
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
    end
    
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

    fprintf(fid,'Output: Node: All Start: 0\n');
    fprintf(fid,'Population: \n');
    fprintf(fid,'Dendrite:  \n');
    fprintf(fid,'Propag: 1 \n');
    fprintf(fid,'Couple:  \n');

    fclose(fid);
    
    fprintf(1,'Integration time: %d s sampled at %d Hz\nGrid size %dx%d, outputting all nodes\nSimulating...',int_time,1/deltat,grid_edge,grid_edge);
end
