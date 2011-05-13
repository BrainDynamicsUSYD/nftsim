function levels=logcontour(dataforplot,perdecade)
%function levels=logcontour(dataforplot,perdecade)
%logcontour returns a vector of contour levels
%with log spaced contours levels based on the data
%dataforplot. The parameter perdecade sets the
%number of contours per log10 decade. If 
%the minimum value of dataforplot is zero the lowest
%contour is at 1E-15.

minval=min(min(dataforplot));
if(0==minval);
  zlimlo=-15;
else
  zlimlo=ceil(log10(minval));
end
zlimhi=floor(log10(max(max(dataforplot))));
levels=[10^zlimlo];
for i=(zlimlo+1):1./perdecade:zlimhi;
  levels=[levels,10^i];
end;
