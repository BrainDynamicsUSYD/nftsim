function y = phiephinkw(gamma, Gee, Gei, Ges, Gse, Gre, Gsr, Grs, Gsn, t0, alpha, beta, omega, k, re)
% Function to calculate ratio of Phi_e/Phi_n in k, omega for EIRS model
tempexp = exp(complex(0.,1.) .* omega .* t0 ./ 2.0);
tempL = L(alpha,beta,omega);
Sd = Ges * Gse
Si = Ges * Gsr * Gre
Sr = Gsr * Grs
tempq2r2 = q2r2(gamma, Gei, Gee, Sd, Si, Sr, t0, alpha, beta, omega);
numer = (Ges .* tempL) .* Gsn .* tempL .* tempexp;
denom = (1.0 - Gei .* tempL) .* (1.0 - Sr .* tempL .* tempL) .* tempq2r2;
prefact = numer ./ denom;
prefact = prefact * ones(1,length(k));
tempq2r2 = tempq2r2 * ones(1,length(k));
k2 = ones(length(omega),1) * (k.*k);
form = 1./(tempq2r2 + k2*re*re);
y = prefact .* form;

function y = q2r2(gamma, Gei, Gee, Sd, Si, Sr, t0, alpha, beta, omega)
% Calculate q^2r^2 term in spectral form for EIRS model
tempL = L(alpha,beta,omega);
tempexp = exp(complex(0.,1.).* omega .* t0);
temp = Gee + tempexp.*((Sd + Si .* tempL) .* tempL) ./ (1 - Sr .*tempL .* tempL); 
y= (1.0 - complex(0.,1.) .* omega ./ gamma).^2 - tempL ./ (1 - Gei .* tempL).*temp;

function y = L(alpha,beta,omega)
% Calculate the dendritic function in spectral space
y= 1 ./((1 - complex(0,1) .* omega / alpha) .* (1 - complex(0,1) .* omega / beta));
