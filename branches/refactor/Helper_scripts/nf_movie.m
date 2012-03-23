function nf_movie( nf, field, fname )
    % Script to generate a movie of a field with spatial variation
    % This script assumes the nodes are continuous, and the grid is a square
    %
    % nf_movie( nf, field, avi )
    %   field is a string of a field name e.g. "Propag.2.phi"
    %   avi is optionally a string filename to save the movie 
    %
    % Felix Fung 120322

    [data,side] = nf_grid(nf,field);
    [X,Y] = meshgrid(1:side,1:side);

    figure; h = surf( X, Y, data(:,:,1) );
    zlim([0 1]);
    shading interp; lighting gouraud; camlight;
    F(nf.npoints) = getframe(gcf); % Trick to preallocate F

    for t = 1:nf.npoints
        norm = max(max(data(:,:,t)));
        set( h, 'ZData', data(:,:,t)/norm );
        title( ['Time: ',num2str(nf.deltat * t)] );
        F(t) = getframe(gcf);
    end
    
    % Output a movie if an avi filename was specified
    if ~(nargin < 3 || isempty(avi))
        avi = avifile(fname);
        avi = addframe( avi, F );
        close(avi);
    end
end
