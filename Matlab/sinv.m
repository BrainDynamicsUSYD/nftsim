function out=sinv(phi,theta,sigma,qmax)
    % Inverse sigmoid function
    % firing rate -> voltage
	if nargin == 2
		sigma = theta.sigma;
		qmax = theta.qmax;
		theta = theta.theta;
	end
	
% inverse sigmoid function
    out=theta - sigma*log(qmax./phi - 1);
    out(imag(out)~=0)=NaN;
end
