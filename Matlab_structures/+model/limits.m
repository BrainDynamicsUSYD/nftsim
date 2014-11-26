function lim = limits(parmode,state);
	% Return limits for parameters in the model
	% lim = limits(parmode,state)
	%
	% This function exists to provide a single place to set these ranges
	% parmode = 1 for gab mode
	% parmode = 2 for nuab mode
	% parmode = 3 for phi mode
	% 
	% state = 0,1 for all limits (pdb_all - 0 and 1 implement different constraints in validate.m)
	% state = 2 for wake limits
	% state = 3 for sleep limits
	% state = 4 for trajectory limits (union of wake and sleep limits)
	%
	% Limits on the gains are the same in all states
	% Wake and sleep control the nuab and phia limits
	
	if nargin < 2
		state = 1; % Limits corresponding to pdb_all
	end
	
	switch parmode
		case 1 % Gab
		
			%		gee   gei   ges  gse   gsr  gsn  gre   grs
		    lim = [  0    -20    0    0   -20    0   0      0; ... 
            		20    -0    20   20   -0    20   20    20];


	 	case 2 % nuab
		    switch state
			    case {0, 1}
				    % pdb_all aka pdb_all
					%		vee    vei    ves   vse   vsr     vsn   vre   vrs
					lim = [ 0.3    -60    0.02  0.03  -32     0.01  0.07  0.02; ...
							60   -0.04   40    48    -0.02   40    6    7];	
				case 2
				    % pdb_wake. Analytic constraints of the form in thesis page 17
					%		vee    vei    ves   vse   vsr     vsn   vre   vrs
		            lim = [ 0.3    -16    0.02  0.03  -7     0.01  0.07    0.02; ...
		            		16   -0.04   16    16    -0.02   16    0.47    0.45];		
				case 3	
				    % pdb_sleep. Analytic constraints as before
					%		vee    vei    ves   vse   vsr     vsn   vre   vrs
					lim = [ 0.3    -16    0.02  0.03  -32     0.01  0.07  0.02; ...
							16   -0.04   16    48    -0.02   40    1.4    4.5];	
				case 4
					lim = [ 0.3    -16    0.02  0.03  -32     0.01  0.07  0.02; ...
							16   -0.04   16    48    -0.02   40    1.4    4.5];	
			end

			lim = lim * 1e-3; % Provide the limits in V, not mV	
	
		case 3 % phi
		    switch state
			    case {0, 1, 4}
					% 	    e    r     s
					lim = [ 0.1  0.1  0.1;...
					        60  60  60];
				case 2
					% 	    e    r     s  waking firing rates
					lim = [ 5   2    5;...
					       20  20   20];
				case 3
					% 	    e    r     s  sleeping firing rates
					lim = [ 5   20    0.3;...
					       20  40    3];
		       case 4
			       	% 	    e    r     s  sleeping firing rates
			       	lim = [ 5   20    0.3;...
			       	       20  40    20];
			end
					lim = [ eps eps eps; 339.9 339.9 339.9]; % All firing rates are OK
		      		%lim = [ 25 eps eps; 300 339.9 339.9]; % Restrict xlimit_stage7 to known VE range for X>1
	end
