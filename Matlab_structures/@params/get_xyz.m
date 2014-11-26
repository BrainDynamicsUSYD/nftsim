function [X,Y,Z,Zp,L,M] = get_xyz(self,w,x,y)

    % Now, calculate Mes/Mse assuming symmetric delays for CT/TC
    if isfunction(self.spatial_t0)
        spec_t0 = self.spatial_t0(x,y);
    else
        spec_t0 = self.t0 * ones(size(x));
    end
    M = exp(1i*bsxfun(@times,w,spec_t0/2)); % This is gridded

    % Spatial XYZ will only use the first alpha/beta spatial function!
    assert(all(self.alpha==self.alpha(1)))
    assert(all(self.beta==self.beta(1)))
    %assert(length(self.spatial_alpha) < 2);
    %assert(length(self.spatial_beta) < 2);

    if ~isempty(self.spatial_alpha) && isfunction(self.spatial_alpha{1})
        a = self.spatial_alpha{1}(x,y);
    else
        a = self.alpha(1)*ones(size(x));
    end

    if ~isempty(self.spatial_beta) && isfunction(self.spatial_beta{1})
        b = self.spatial_beta{1}(x,y);
    else
        b = self.beta(1)*ones(size(x));
    end

    a1 = bsxfun(@rdivide,1i*w,a);
    b1 = bsxfun(@rdivide,1i*w,b);
    L = 1./((1-a1).*(1-b1));

	if isempty(self.spatial_xyz)
		X = self.xyz(1)*ones(size(L));
		Y = self.xyz(2)*ones(size(L));
		Z = self.xyz(3)*ones(size(L));
	else
		if isfunction(self.spatial_xyz{1})
			tmp = self.spatial_xyz{1}(x,y);
    		X = bsxfun(@times,ones(size(w)),tmp);
    	else
    		X = self.xyz(1)*ones(size(L));
    	end
		if isfunction(self.spatial_xyz{2})
			tmp = self.spatial_xyz{2}(x,y);
			Y = bsxfun(@times,ones(size(w)),tmp);
		else
			Y = self.xyz(2)*ones(size(L));
		end
		if isfunction(self.spatial_xyz{3})
			tmp = self.spatial_xyz{3}(x,y);
			Z = bsxfun(@times,ones(size(w)),tmp);
		else
			Z = self.xyz(3)*ones(size(L));
		end
	end
	
	Zp = bsxfun(@times,Z,((a+b).^2)./(a.*b));
