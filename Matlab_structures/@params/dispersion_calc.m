function [stab,f,P,stab_w,q2] = dispersion_calc(p,check_stability)
    % TODO: Plenty of performance optimizations
    if nargin < 2
        check_stability = 0;
    end

    if check_stability % Checks if *linearly* stable
        stab_w=2*pi*linspace(0,200,10000); % High res array, 0 to 200Hz with 10000 points
        [Jee,Jei,Jese,Jesre,Jsrs,Jesn] = get_Jabcd(p,stab_w);
        d=((1-1i*stab_w/p.gammae).^2.*(1-Jei)-Jee).*(1-Jsrs)-(Jese + Jesre);      
        stab=d(1)>0 && ~any(real(d(2:end))<0 & imag(d(2:end)).*imag(d(1:end-1))<0);

        if nargout >= 4
            %figure(2)
            %plot(real((Jese + Jesre)),imag((Jese + Jesre)));
            q2 = ((1-1i*stab_w./p.gammae).^2-(1./(1-Jei)).*(Jee + (Jese + Jesre)./(1-Jsrs)))./p.re^2;
        end

    else
        stab = NaN; % Stability was not examined
    end

    if nargout > 3 % If q2 is required
        fmax = 100;
        nw = 1000;
    else
        fmax = 45;
        nw = 1000; % Sufficient for spectrum
    end

    if nargout < 2 % If only stability is required
        return
    end
    
    %f = linspace(0,fmax,nw); % It is critical to start at w=0 for the convolution - but no convolutions anymore
    f = linspace(0,45,nw); % It is critical to start at w=0 for the convolution - but no convolutions anymore
    w = f*2*pi;
    dw = w(2)-w(1);

    kmax = 6;
    Lx = 0.5; % linear dimensions of cortex (assume square)
    k0 = 10; % Volume conduction parameter
    dk = 2*pi/Lx;
    m_rows = -kmax:kmax; 
    n_cols = -kmax:kmax; 
    [kxa,kya] = meshgrid(dk*m_rows,dk*n_cols); 
    k2 = kxa.^2+kya.^2;
    k2u = unique(k2(:));
    k2u = [k2u histc(k2(:),k2u)]; % First col is the k2 value, second is how many times it appeared

    [Jee,Jei,Jese,Jesre,Jsrs,Jesn] = get_Jabcd(p,w);
    
    P = zeros(size(w));
    %phie = zeros(size(k2,1),size(k2,2),length(w));
    %Vs = zeros(size(k2,1),size(k2,2),length(w));

    Dew = (1-1i*w./p.gammae).^2;
%    Diw = 1;
%    Drw = 1;
%    Dsw = (1-1i*w./p.gammas).^2;
%    Dnw = (1-1i*w./p.gamman).^2;

    [De,Di,Dr,Ds,Dn] = deal(1);

    %for m = 1:length(m_rows)

    %    for n = 1:length(n_cols)
    %        k = k2(m,n);
        for j = 1:size(k2u,1)
            k = k2u(j,1);

            De = Dew + k*p.re.^2;
            %Ds = Dsw + k*(p.rs).^2;
            %Dn = Dnw + k*(p.rn).^2;
            
            A = De.*(Di-Jei)./Di - Jee;
            %B = (Ds - Jese./A - Jesre./A./Dr - Jsrs./Dr);

            %this_phis = Jsn.*(p.phin./Dn)./B;
            %this_phie = this_phis.*Jes./A;
            % if 0 && k == 0
            %         q = A.*(1-Jsrs) - Jese - Jesre;
            %         q = A.*(1-Jsrs)-Jese;
            %         [a,b] = min(abs(q)); 
            %         figure(3)
            %         %hold on
            %         %plot(f,q.*q.'')
            %         %plot(f,real(q)+imag(q))
            %         fn = @real;
            %         plot(f,fn(q),'b')
            %         %hold all
            %         %plot(f,fn(q),'g--')
            %         %plot3(real(q),imag(q),f);
            %         %set(gca,'View',[0 90])
            %         %xlabel('Re'); ylabel('Im')
            %         plot(f,fn(A.*(1-Jsrs)),'r--');
            %         plot(f,fn(-Jese),'g--');
            %         %plot(f,fn(A.*(1-Jsrs))+fn(-Jese),'k--');
            % end
            this_phie = Jesn.*p.phin./(A.*(1-Jsrs) - Jese - Jesre);
            
            %this_phie = Jsn.*Jes.*p.phin./(A.*(1-Jsrs) - Jesre);
            %this_phie = Jsn.*Jes.*p.phin./(A.*(1-Jsrs));

            %this_phir = (Fre.*this_phie.*M+Frs.*this_phis)./Dr;
            
            %phie(m,n,:) = this_phie;
            %phis(m,n,:) = this_phis;
            %phir(m,n,:) = this_phir;

            %Ve(m,n,:) = this_phie.*De/rho1(p.phia(1),p);
            %Vr(m,n,:) = phir(m,n,:)/rho1(p.phia(2),p);
            %Vs(m,n,:) = this_phis.*Ds/rho1(p.phia(3),p);

            %P = P + abs(this_phie).^2 ;
            P = P + k2u(j,2).*abs(this_phie).^2 * exp(-k/k0^2); % For use with the efficient way
            %P = P + k2u(j,2).*abs(this_phie).^2; % For use with the efficient way, NO VOLUME CONDUCTION
            %P = P + abs(this_phie).^2 * exp(-k/k0^2); % For use with the full calculation
        end

    %end    

    P = P.*dk.^2; % Multiply by dk then sum to get the integral over k
    P = P(:).'*2*pi; % Convert to P(f)

function [Jee,Jei,Jese,Jesre,Jsrs,Jesn] = get_Jabcd(p,w)
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
