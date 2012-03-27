function nf_movie( nf, field, normalize, fname )
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

    datamean = zeros(1,nf.npoints);
    for t = 1:nf.npoints
        datamean(t) = mean(mean(data(:,:,t)));
    end

    plotdata = data;
    switch normalize
        case 1
            for t = 1:nf.npoints
                plotdata(:,:,t) = data(:,:,t) -mean(mean(data(:,:,t)));
            end
        case 2
            for t = 1:nf.npoints
                threshold = zeros(1,nf.npoints);
                norm = zeros(1,nf.npoints);
                threshold(t) = min(min(data(:,:,t)));
                norm(t) = max(max(data(:,:,t)-threshold(t)));
                plotdata(:,:,t) = ( data(:,:,t)-threshold(t) )./norm(t);
            end
    end

    figure; h = surf( X, Y, plotdata(:,:,1) ); title('','Interpreter','none');
    zlim([min(plotdata(:)) max(plotdata(:))]);

    shading interp; lighting gouraud; camlight;
    F(nf.npoints) = getframe(gcf); % Trick to preallocate F

    for t = 1:nf.npoints
        set( h, 'ZData', plotdata(:,:,t) );
        title( [field,...
            ': Time: ',num2str(nf.deltat * t),...
            ', Mean: ',num2str(datamean(t))],'Interpreter','none' );
        pause(.05);
        if ~(nargin < 4 || isempty(fname)) F(t) = getframe(gcf); end
    end

    % Output a movie if an avi filename was specified
    if ~(nargin < 4 || isempty(fname))
        avi = avifile(fname);
        avi = addframe( avi, F );
        avi = close(avi);
    end
end
