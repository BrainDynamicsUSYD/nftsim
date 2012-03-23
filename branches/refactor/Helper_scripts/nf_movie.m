% Script to generate a movie of a field with spatial variation
% This script assumes the nodes are continuous, and the grid is a square
% Felix Fung 120322

function nf_movie( nf, field, avi )

raw = nf_extract(nf,[],field);
side = sqrt(max(nf.nodes{1})); % length of square grid
deltat = nf.time(2)-nf.time(1);
second = 0;

field = zeros( side, side );
[X,Y] = meshgrid(1:side,1:side);

figure; h = surf( X, Y, field(:,:) );
title( ['Time: ',num2str(second)] );
zlim([0 1]);
shading interp; lighting gouraud; camlight;

if ~(nargin < 3 || isempty(avi)) 
    avi = avifile(avi); 
end

for t = 1:length(nf.time)
    field(:,:) = reshape( raw(t,:), side, side );
    norm = max(max(field));
    field = field./norm;
    set( h, 'ZData', field(:,:) );
    second = second+deltat; title( ['Time: ',num2str(second)] );
    F = getframe(gcf);
    if ~(nargin < 3 || isempty(avi)) 
        avi = addframe( avi, F );
    end
end
    if ~(nargin < 3 || isempty(avi)) 
        avi = close(avi);
    end

end
