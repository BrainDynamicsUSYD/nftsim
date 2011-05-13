function [Gee, Gei, Ges, Gse, Gre, Gsr, Grs, Gsn] = calcgains(nuee, nuei, nues, nuse, nure, nusr, nurs, nusn, sigma, qmax, phie, phir, phis)
% Function to calculate cortical-thalamic model gains from nu values
Gee = dSdV(phie, sigma, qmax)*nuee;
Gei = dSdV(phie, sigma, qmax)*nuei;
Ges = dSdV(phie, sigma, qmax)*nues;
Gse = dSdV(phis, sigma, qmax)*nuse;
Gre = dSdV(phir, sigma, qmax)*nure;
Gsr = dSdV(phis, sigma, qmax)*nusr;
Grs = dSdV(phir, sigma, qmax)*nurs;
Gsn = dSdV(phis, sigma, qmax)*nusn;

function y=dSdV(phi, sigma, qmax)
% Function to calculate slope of sigmoid
y = (phi/sigma)*(1 - phi/qmax);
