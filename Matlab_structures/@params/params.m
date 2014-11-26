classdef params < matlab.mixin.Copyable
	properties
		Lx 			= 0.5;
		Ly 			= 0.5;
		alpha       = [];
		beta        = [];
		t0          = 85e-3;
		taues       = 0.0425; % t0/2
		tause       = 0.0425;
		qmax        = 340;
		gammae      = 116;
		re          = 86e-3
		gammas      = 1.2e4;
		rs          = 1.5e-3;			% From page 78 of Thalamus I
		gamman      = 1.2e4;
		rn          = 2e-3; 			% From page 64 of 'Exploring the Thalamus', multiplied by 10 to get cortical map units 	
		sigma       = 3.8e-3; 			%	Threshold spread (V)
		theta       = 0.0129 			%	Default is 3.4*sigma. Threshold ratio * sigma (Nondimensional)
		phin        = 1e-5;				%	Stimulus effective firing rate (s^-1)
		 
		nus         = [];
		gab         = [];
		gabcd       = [];
		xyz         = [];
		ve          = [];
		phia        = [];
		nroots      = 0;
		stab        = NaN
		emg_a       = 0;

		spatial_t0  = 0;
		spatial_nus = {};
		spatial_gab = {};
		spatial_gabcd = {};
		spatial_alpha = {}; % If there is only one element in this array, it is used for all of the L's
		spatial_beta = {}; 
		spatial_xyz = {}; % Note that XYZ takes precedence if it is defined

		disable_set = true; % This flag stops any additional code in the set functions from running
	end

	methods

		function self= params(varargin)
			% First argument is alphamode OR a point struct
			% alphamode= 1 -> All wake
			% alphamode= 0 -> All sleep

			% Handle construction from a point struct
			if nargin > 0 && isstruct(varargin{1})
				p= varargin{1};
				fieldname = fields(p);
				for j = 1:length(fieldname)
					switch fieldname{j} % Handle some mistakes made earlier with field names changing
						case 'ves'
							self.ve = p.ves;
						case 't0_amp'
							continue
						otherwise
							self.(fieldname{j})= p.(fieldname{j});
					end
				end
				self.complete_gab(1);
				self.disable_set= false;
				return
			end

			if nargin < 1 || isempty(varargin{1})
				alphamode = 1; % Wake by default
            else
                alphamode = varargin{1};                
			end

			% More alphamodes in Revision 785
			if alphamode== 1
				av= 1/12e-3; %	1/Decay time (s^-1) WAKE
				bv= 1/1.3e-3; %	1/Rise time (s^-1)  WAKE
			else
				av= 45;
				bv= 185;
			end

			self.alpha= ones(1,8)*av;
			self.beta= ones(1,8)*bv;

			self.disable_set= false;
		end

		function set.nus(self,val)
			self.nus= val;

			if self.disable_set
				return
			end

			if ~isempty(val)
				if ~isempty(self.phia)
					self.complete_gab(1);
				else
					self.complete_gab();
				end
			end
		end

		function set.gab(self,val)
			self.gab= val;

			if self.disable_set
				return
			end

			if ~isempty(val)
				if ~isempty(self.phia)
					self.complete_nuab(1);
				else
					self.complete_nuab();
				end
			end
		end

		function keep_solution(self,n)
			% Use complete_gab to keep only one of the solutions
			self.phia= self.phia(n,:);
			self.complete_gab(1);
		end

	end
end