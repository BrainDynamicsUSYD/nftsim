function fcn = variation_function(self,var_type,varargin)
	% This function serves as a container for various patterns of spatial make_variation
	% that can be generically applied to different parameters by wrapper functions
	% varargin{1} is always the current value of the parameter in question
	% then the remaining arguments were provided by the parent or user
	switch var_type
		case 'cosine'
			fcn = cosine(self.Ly,varargin{1:end}); % first two arguments only
		case 'gaussian'
			fcn = gaussian(varargin{1:end})
		case 'single_step'
			fcn = single_step(self.Ly,varargin{1:end})
	end
end

function fcn = cosine(Ly,par,val)
	% One variation across the length of the scalp
	fcn = @(x,y) par + val*cos(y*2*pi/Ly);
end

function fcn = gaussian(par,p1,x1,y1,sig)
	fcn = @(x,y) par + p1.*exp(( -(x-x1).^2 -(y-y1).^2)/(2*sig.^2));
end

function fcn = single_step(Ly,par,offset)
	fcn = @(x,y) par + offset*(y>Ly/2);
end