function complete_nuab(p,neighbour)
    % Add nu_ab to a points struct
    %
    % complete_nuab(p)
    %
    % Given a point struct p without nuab, fill them in
    % This will overwrite existing p.nus values
    % Input point must have only one set of gab
    if nargin < 2
    	neighbour = 0;
    end

    p.disable_set = true;

    p.nus = [];
    p.nroots = [];

    if size(p.gab,1) > 1 % Test to make sure that only one set of Gab has been provided
		p.disable_set = false;
		return
	end



	% for the root finding
	philim = model.limits(3);
	
	phi = [philim(1,1)-0.1:0.01:philim(2,1)]; % Search over a limited range
	
	gee=p.gab(1);
	gei=p.gab(2);  
	ges=p.gab(3);  
	gse=p.gab(4);  
	gsr=p.gab(5);  
	gsn=p.gab(6);  
	gre=p.gab(7);  
	grs=p.gab(8);

	% get qmax, theta, sigma, phin
	qmax=p.qmax;
	theta=p.theta;
	sigma=p.sigma;
	phin=1;

	% if x,y,z missing, fill them
	if (~isfield(p,'xyz') || isempty(p.xyz))
		xyzs = zeros(1,3);
	    xyzs(1)=gee/(1-gei);
	    xyzs(2)=(ges*gse+ges*gsr*gre)/((1-gsr*grs)*(1-gei));
	    xyzs(3)=-gsr*grs*p.alpha(5)*p.beta(5)/(p.alpha(5)+p.beta(5))^2;
		p.xyz = xyzs;
	end

	p.stab= p.dispersion_calc(1); 

	% Initialise the functions to calculate the phi values
    function out=fp_phie_root(phie)
    % f(phie)=0
        out=gre*phie+grs*fp_phis(phie)-rhov(fp_phir(phie));
    end
    function out=fp_phis(phie)
    % calculate phis from value of ve at fixed-point
        out=(rhov(phie)-(gee+gei)*phie)/ges;
    end
    function out=fp_phir(phie)
    % calculate phir from value of ve at fixed-point
        out=(rhov(fp_phis(phie))-gse*phie-gsn*phin)/gsr;
    end
    function out=rhov(phi)
    % calculate rho*v in terms of phi
        out=rho(phi).*sinv(phi,p.theta,p.sigma,p.qmax);
    end
    function out=rho(phi)
    % calculate rho in terms of phi
    % uses sig, qmax from above
        out=phi.*(1-phi/qmax)/sigma;
    end

	% Get the firing rates
	fn=fp_phie_root(phi);
	%plot(phi,fn)			
	%phie = allroots(fn,phi); % No root polishing
	
	phie = allroots(@fp_phie_root,phi); % Root polishing- may break due to discontinuities?
	phir = fp_phir(phie);
	phis = fp_phis(phie);
	
	phiv = [phie',phir',phis']; % This accounts for variable number of roots
	ves = sinv(phie,p.theta,p.sigma,p.qmax);

	if neighbour && size(p.phia,1) == 1
		% Pick the solution closest to the original phis
		dist = [phiv(:,1)-p.phia(1),phiv(:,2)-p.phia(2),phiv(:,3)-p.phia(3)].^2;
		[d,ind] = min(sum(dist,2));
		% Select only the closest match
		ves = ves(ind);
		phiv = phiv(ind,:);
	end

	p.phia = phiv;
	p.ve = ves;

	p.nroots=length(p.ve);
	p.nus=zeros(p.nroots,8);
	
	% Calculate nuab for each solution set
	for j=1:p.nroots
	    rhoe=rho(p.phia(j,1));
	    rhor=rho(p.phia(j,2));
	    rhos=rho(p.phia(j,3));
	    rhoset=[rhoe rhoe rhoe rhos rhos rhos rhor rhor]; 
	    p.nus(j,:)=p.gab./rhoset; 
	end

	p.disable_set = false;

end

