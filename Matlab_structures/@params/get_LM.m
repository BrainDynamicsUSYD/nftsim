function [L,M] = get_LM(self,w,x,y)
    L = cell(1,8);
    if isempty(self.spatial_alpha) && isempty(self.spatial_beta) 
        % Normal case
        L_fcn = @(idx) bsxfun(@times,1./((1-1i*w/self.alpha(idx)).*(1-1i*w/self.beta(idx))),ones(size(x)));
        for j = 1:8
            L{j} = L_fcn(j);
        end
    else % Go through and evaluate each of them
        for j = 1:8
            % What is this L's alpha?
            if ~isempty(self.spatial_alpha) && isfunction(self.spatial_alpha{j})
                a = self.spatial_alpha{j}(x,y);
            else
                a = self.alpha(j)*ones(size(x));
            end

            if ~isempty(self.spatial_beta) && isfunction(self.spatial_beta{j})
                b = self.spatial_beta{j}(x,y);
            else
                b = self.beta(j)*ones(size(x));
            end
            
            a1 = bsxfun(@rdivide,1i*w,a);
            b1 = bsxfun(@rdivide,1i*w,b);

            L{j} = 1./((1-a1).*(1-b1));
        end
    end
    
    % Now, calculate Mes/Mse assuming symmetric delays for CT/TC
    if isfunction(self.spatial_t0)
        spec_t0 = self.spatial_t0(x,y);
    else
        spec_t0 = self.t0 * ones(size(x));
    end
    M = exp(1i*bsxfun(@times,w,spec_t0/2)); % This is gridded

    %keyboard