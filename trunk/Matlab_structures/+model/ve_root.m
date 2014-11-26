function y=ve_root(ve,nus,theta,sigma,qmax)
    % Find solutions for the steady state equation
    % This function is a performance bottleneck- consider almost identical mex_ve_root.c instead

	y=sigm(nus(4)*sigm(ve,theta,sigma,qmax)+nus(5)*sigm(nus(7)*sigm(ve,theta,sigma,qmax)+(nus(8)/nus(3))*(ve-(nus(1)+nus(2))*sigm(ve,theta,sigma,qmax)),theta,sigma,qmax)+nus(6),theta,sigma,qmax)+((nus(1)+nus(2))*sigm(ve,theta,sigma,qmax)-ve)/nus(3);
	
