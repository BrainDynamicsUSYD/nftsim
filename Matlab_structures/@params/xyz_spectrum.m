function [f,P,stab] = xyz_spectrum(self,f)
	% Formula from Roberts 2012 (Corticothalamic dynamics...)
	if nargin < 2 || isempty(f)
		f=linspace(0,100,10000);
	end
	w = f*2*pi;
	
	L2=1./((1-1i*w/self.alpha(1)).*(1-1i*w/self.beta(1))).^2; 
    Zp = self.xyz(3)*(self.alpha(1)+self.beta(1)).^2./(self.alpha(1)*self.beta(1));
    q2 = (1-1i*w/self.gammae).^2 - self.xyz(1) - self.xyz(2).*exp(1i*w*self.t0).*(1+Zp)./(1+Zp.*L2); % My eq 3.30
		
	% Check for crossing the imaginary axis at negative real Q^2
    stab = ~any(imag(q2(2:end)).*imag(q2(1:end-1))< 0 & real(q2(1:end-1)) < 0); % S

    P = 1./(abs(1+Zp.*(L2)).^2).*((1./abs(q2)).^2);
    
    
