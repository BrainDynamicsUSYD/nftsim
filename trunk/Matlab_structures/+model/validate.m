function valid = validate(pars,task,state) 
	% Validate a set of points
    % valid = validate(pars,task,state) 
    %
    % IF PARS IS A POINT OBJECT, ALL TESTS ARE RUN AND THE TOTAL VALIDITY RETURNED
	% This function makes it easy to check if a random
	% combination of nuab (or gab, phi or xyz) satisfy
	% a set of criteria. Specifically, it makes it easy
	% to check both single and paired constraints on nuab
	%
	% validate(pars,task)
	% task: 1=gab, 2=nuab, 3=phi
    % state =
    %       0 - No pair filtering
    %       1 - Pair filtering for 0.1 < phia < 60
    %       2 - Pair filtering for wake (+ phia)
    %       3 - Pair filtering for sleep (+ phia)
    %
    % Code to validate XYZ is present in Rev 640
	if nargin < 3 && task ~= 1
        warning('Default state was used- this probably should not happen');
        state = 1; % pdb_all constraints by default
    end

    if isstruct(pars)
        valid = all([model.validate(pars.gab,1,state),model.validate(pars.nus,2,state),model.validate(pars.phia,3,state)]);
        return
    end
    
	valid = 1; % Default points are valid. They become invalidated if they
	% a) Fail a test
	% b) None of the cases run for some reason

	switch task
		case 1 % Validate gab
			lim = model.limits(1);
			if sum(pars <= lim(1,:)) + sum(pars >= lim(2,:)) % Single limits
				valid = 0;
			end
			return	
					
		case {2,4} % Validate nuab
			lim = model.limits(2);
			if sum(pars <= lim(1,:)) + sum(pars >= lim(2,:)) % Single limits
				valid = 0;
				return
            end
            
			% Pair constraints based on pdb2 with correct sigma value. Analytic constraint on nuee+nuei
			if ...
			    state > 0 && ( (pars(2) >= (-1*pars(1) + 1.5e-4)) || ...
			    (pars(7) >= (-0.06*pars(1)+6.6e-3)) || ...
			    (pars(7) >= (-0.07*pars(2)+2.8e-3)) || ...
			    (pars(7) >= (6e-3*exp(6e2*pars(5)+2.4)+0.7e-3)) )
			    
				valid = 0;
				return
			end
			
			switch state
				case 2 % Apply the wake filter
					if ...
					    pars(4) >= (-1*pars(3) + 17e-3) || ...
					    pars(3) >= (-1*pars(1)+15e-3) || ...
					    pars(3) >= (-1*pars(2)+4e-3) || ...
					    pars(8) >= (-1*pars(7)+4.9e-4) || ...
					    pars(5) >= (-0.3*pars(4)+4e-4) || ...
					    pars(5) <= (-0.7*pars(4)-2e-3) || ...
					    pars(5) <= (0.4*pars(3)-7.7e-3)
						
						valid = 0;
						return
					end
				case 3 % Apply the sleep filter
					if ...
                        pars(5) <= (1.2*pars(1) - 3.3e-2) || ...
                        pars(5) <= (-1.2*pars(2) - 3.3e-2) || ...
                        pars(5) <= (1.6*pars(3) - 3.8e-2) || ...
                        pars(5) >= (-0.12*pars(4) + 1e-4) || ...
                        pars(5) <= (-0.85*pars(4) - 5e-3) || ...
                        pars(8) >= (-3*pars(7) + 5e-3)
                        
						valid = 0;
						return
					end
			end
			
			return
						
		case 3 % Validate a set of phi
			lim = model.limits(3,state);
			phitest = sum(bsxfun(@le,pars,lim(1,:)) + bsxfun(@ge,pars,lim(2,:)),2);
            valid = phitest == 0;
			return
	end
	
	valid = 0; % Fallback case. Each 'case' in the switch block should return individually
    % This means that if something unexpected happens the point is classified invalid
	return
    
end
