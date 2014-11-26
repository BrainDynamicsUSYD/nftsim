function apply_variation(self,par,variation_type,varargin)
	% Params can be
	% t0, alpha, beta, alpha_ee, beta_ee, g_ee, nu_ee, g5_ese 
	% variation_function gets the current value of the parameter in question, plus all of the remaining arguments
	if strcmp(par,'t0')
		self.spatial_t0 = self.variation_function(variation_type,self.t0,varargin{1:end});
		return
	end

	idx_strs = {'ee','ei','es','se','sr','sn','re','rs','ese','esre','srs'};
	if strfind(par,'_')
		s = regexp(par,'_','split');
		par = s{1};
		idx = find(strcmp(s{2},idx_strs));
		if isempty(idx)
			error('Index could not be found')
		end
	else
		idx = []; % Shortcut for setting all alphas and betas
	end

	switch par
		case 'g'
			if isempty(self.gab)
				error('Cannot apply a spatial variation without parameter value already being defined');
			end
			if isempty(self.spatial_gab)
				self.spatial_gab = cell(1,8);
			end
			self.spatial_gab{idx} = self.variation_function(variation_type,self.gab(idx),varargin{1:end});
		
		case 'g5'
			if isempty(self.gabcd)
				error('Cannot apply a spatial variation without parameter value already being defined');
			end
			if isempty(self.spatial_gabcd)
				self.spatial_gabcd = cell(1,5);
			end
			if idx > 2
				idx = idx - 6; % Note that this turns the loop gains tacked on to the end of idx_strs into indexes 3-5
			end

			self.spatial_gabcd{idx} = self.variation_function(variation_type,self.gabcd(idx),varargin{1:end});

		case 'nu'
			if isempty(self.nus)
				error('Cannot apply a spatial variation without parameter value already being defined');
			end
			if isempty(self.spatial_nus)
				self.spatial_nus = cell(1,8);
			end
			self.spatial_nus{idx} = self.variation_function(variation_type,self.nus(idx),varargin{1:end});
		
		case 'alpha'
			if isempty(self.spatial_alpha)
				self.spatial_alpha = cell(1,8);
			end
			if isempty(idx)
				idx = 1:8;
			end

			for j = idx
				self.spatial_alpha{j} = self.variation_function(variation_type,self.alpha(j),varargin{1:end});
			end

		case 'beta'
			if isempty(self.spatial_beta)
				self.spatial_beta = cell(1,8);
			end
			if isempty(idx)
				idx = 1:8;
			end

			for j = idx
				self.spatial_beta{j} = self.variation_function(variation_type,self.beta(j),varargin{1:end});
			end

		case 'X'
			if isempty(self.spatial_xyz)
				self.spatial_xyz = cell(1,3);
			end

			self.spatial_xyz{1} = self.variation_function(variation_type,self.xyz(1),varargin{1:end});
		
		case 'Y'
			if isempty(self.spatial_xyz)
				self.spatial_xyz = cell(1,3);
			end

			self.spatial_xyz{2} = self.variation_function(variation_type,self.xyz(2),varargin{1:end});
		
		case 'Z'
			if isempty(self.spatial_xyz)
				self.spatial_xyz = cell(1,3);
			end

			self.spatial_xyz{3} = self.variation_function(variation_type,self.xyz(3),varargin{1:end});

		otherwise
			error('Unrecognized parameter')
	end
