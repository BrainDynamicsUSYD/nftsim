function stab = is_stable(self)
    % Return stability, using XYZ if spatial_xyz is present or there are no gains
    stab_w=2*pi*linspace(0,200,10000); % High res array, 0 to 200Hz with 10000 points
    
    if ~isempty(self.spatial_xyz) || (isempty(self.gab) && isempty(self.gabcd))
        L2=1./((1-1i*stab_w/self.alpha(1)).*(1-1i*stab_w/self.beta(1))).^2; 
        Zp = self.xyz(3)*(self.alpha(1)+self.beta(1)).^2./(self.alpha(1)*self.beta(1));
        q2 = (1-1i*stab_w/self.gammae).^2 - self.xyz(1) - self.xyz(2).*exp(1i*stab_w*self.t0).*(1+Zp)./(1+Zp.*L2); % My eq 3.30
        stab = ~any(imag(q2(2:end)).*imag(q2(1:end-1))< 0 & real(q2(1:end-1)) < 0);    
    else
        [Jee,Jei,Jese,Jesre,Jsrs,Jesn] = get_jabcd_nospatial(self,stab_w);
        d=((1-1i*stab_w/self.gammae).^2.*(1-Jei)-Jee).*(1-Jsrs)-(Jese + Jesre);      
        stab=d(1)>0 && ~any(real(d(2:end))<0 & imag(d(2:end)).*imag(d(1:end-1))<0);

    end


function [Jee,Jei,Jese,Jesre,Jsrs,Jesn] = get_jabcd_nospatial(p,w)
    L = @(idx) 1./((1-1i*w/p.alpha(idx)).*(1-1i*w/p.beta(idx)));
    M = exp(1i*w*p.taues); % Assume Mes = Mse

    % It is important to use the G_ab if they are available
    % because the G_ab define a value for G_sn which allows the
    % normalization with NeuroField to be correct.
    % The 5 gains do not specify the normalization of the power spectrum
    if isempty(p.gab)
        Jee = L(1).*p.gabcd(1);
        Jei = L(2).*p.gabcd(2);
        Jese = L(3).*L(4).*p.gabcd(3).*M.*M;
        Jesre = L(3).*L(5).*L(7).*p.gabcd(4).*M.*M;
        Jsrs = L(5).*L(8).*p.gabcd(5);
        Jesn = L(3).*L(6); 
    else
        % Use the 8 gains
        Jee = L(1).*p.gab(1);
        Jei = L(2).*p.gab(2);
        Jes = L(3).*p.gab(3).*M;
        Jse = L(4).*p.gab(4).*M;
        Jsr = L(5).*p.gab(5);
        Jsn = L(6).*p.gab(6);
        Jre = L(7).*p.gab(7).*M;
        Jrs = L(8).*p.gab(8);

        Jese = Jes.*Jse;
        Jesre = Jes.*Jsr.*Jre;
        Jsrs = Jsr.*Jrs;
        Jesn = Jes.*Jsn;
    end
