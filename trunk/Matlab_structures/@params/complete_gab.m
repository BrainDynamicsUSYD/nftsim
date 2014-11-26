function complete_gab(p,neighbour,ve_limit,force_stability,state)
	% Add G_ab to a point struct
	%
	% complete_gab(p,neighbour,ve_limit,force_stability,state)
	%
    % Given a point struct p without Gab, fill them in
    % This program assumes that nuab is more recent than gab
    % ie. Any existing gab values will be ignored and overwritten

    % The optional argument neighbour decides behaviour for multiple solutions
    % If neighbour = 1, and there are already phi values in the point, then 
    % the code will pick only the ve_root that is most consistent with the existing
    % firing rates, and the others will be deleted

    % The optional argument velimit is a parameter that specifies the range
    % over which Ve should be searched- dramatically increases performance if 
    % known in advance

    % Optional argument force_stability will ensure dispersion_calc() gets run
    % Otherwise, it may not be run
    % Can return the spectrum if necessary
    
    % Optional argument state specifies which limits from limits.m to use
    % This is passed to limits() via validate()

    if nargin < 5
        state = 1; % The state corresponds to pdb_all, i.e. reasonably physiological
    end
    
	if nargin < 4 || isempty(force_stability)
		force_stability = 0;
	end
	
	if nargin < 3 || isempty(ve_limit)
		ve_limit = [];
	end
	
	if nargin < 2 || isempty(neighbour)
		neighbour = 0;
	end
	
	p.disable_set = true;

    % Clear all of the fields that this function will populate
	p.nroots = 0;
	p.gab = [];
	p.gabcd = [];
	p.xyz = [];
	p.ve = [];
	p.stab = [];

    if isempty(p.nus) || size(p.nus,1) > 1
    	p.disable_set = false;
    	return
    end

	if ~isempty(ve_limit)
		[ves,discard] =allroots(@model.mex_ve_root,(ve_limit(1):0.00004:ve_limit(2)),p.nus,p.theta,p.sigma,p.qmax);
	else
		[ves,discard] =allroots(@model.mex_ve_root,(-0.04:0.00004:0.04),p.nus,p.theta,p.sigma,p.qmax);
	end
		
	if isempty(ves) % If no roots were found
		p.phia = [];
		p.disable_set = false;
		return
	end
	
	% Preallocate space for phis
	phiv = zeros(length(ves),3); % phie,phir,phis

	% Calculate all of the phi values
	for j = 1:length(ves)
		% Calculate the firing rates
	    phiv(j,1)=sigm(ves(j),p.theta,p.sigma,p.qmax);
	    vr=p.nus(7)*phiv(j,1)+(p.nus(8)/p.nus(3))*(ves(j)-(p.nus(1)+p.nus(2))*phiv(j,1));
	    phiv(j,2)=sigm(vr,p.theta,p.sigma,p.qmax);
	    vs=p.nus(4)*phiv(j,1)+p.nus(5)*phiv(j,2)+p.nus(6);
	    phiv(j,3)=sigm(vs,p.theta,p.sigma,p.qmax);
	end
	
	% valid_phi = validate(phiv,3,state);
	% phiv = phiv(valid_phi,:);
 %    ves = ves(valid_phi);
    
	if isempty(phiv) % If all of the firing rates have been rejected
	    p.phia = [];
	    p.disable_set = false;
		return
	end
	
	% Determine whether to calculate all of the solutions, or just one
	% neighbour must be set, and the point must contain exactly one
	% previous set of firing rates
	if neighbour && size(p.phia,1) == 1
		% Pick the solution closest to the original phis
		dist = [phiv(:,1)-p.phia(1),phiv(:,2)-p.phia(2),phiv(:,3)-p.phia(3)].^2;
		[d,ind] = min(sum(dist,2));
		% Select only the closest match
		ves = ves(ind);
		phiv = phiv(ind,:);
	end
	
	% Preallocate storage for other parameters
	gabs = zeros(length(ves),8);
	gabcds = zeros(length(ves),5);
	xyzs = zeros(length(ves),3); % x,y,z
	stab = zeros(1,length(ves));
	
	for j = 1:length(ves) % For each of the Ve roots
	    % Evaluate rho
	    rhoe=phiv(j,1).*(1-phiv(j,1)/p.qmax)/p.sigma;
	    rhor=phiv(j,2).*(1-phiv(j,2)/p.qmax)/p.sigma;
	    rhos=phiv(j,3).*(1-phiv(j,3)/p.qmax)/p.sigma;
	    rhoset=[rhoe rhoe rhoe rhos rhos rhos rhor rhor]; 

		% Calculate the G_ab
	    gabs(j,:) =  p.nus.*rhoset;
	    gabcds(j,:) = [gabs(j,1),gabs(j,2),gabs(j,3)*gabs(j,4),gabs(j,3)*gabs(j,5)*gabs(j,7),gabs(j,5)*gabs(j,8)];
		
		% Calculate stability
		p.gab = gabs(j,:); % Insert current set of Gab being tested
		
		if force_stability || ( model.validate(phiv(j,:),3,state) && model.validate(gabs(j,:),1,state)) % Only run dispersion_calc if the point is valid
			stab(j) = p.dispersion_calc(1);
		else
		    stab(j) = NaN;
        end
        
        % Calculate XYZ
		% NB. gabs(j,1) = gee, gabs(j,2) = gei etc.
		xyzs(j,1)=gabs(j,1)/(1-gabs(j,2));
	    xyzs(j,2)=(gabs(j,3)*gabs(j,4)+gabs(j,3)*gabs(j,5)*gabs(j,7))/((1-gabs(j,5)*gabs(j,8))*(1-gabs(j,2)));
	    xyzs(j,3)=-gabs(j,5)*gabs(j,8)*p.alpha(5)*p.beta(5)/(p.alpha(5)+p.beta(5))^2;
	end
	
	p.ve = ves;
	p.gab = gabs;
	p.gabcd = gabcds;
	p.xyz = xyzs;
	p.phia = phiv;
	p.nroots = length(ves);
	p.stab = stab;
	p.disable_set = false;
	return
end
