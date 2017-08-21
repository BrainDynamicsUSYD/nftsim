%% Script to generate a movie of a field with spatial variation.
%
% This script assumes the nodes are continuous, and the grid is a square.
%
% ARGUMENTS:
%        obj   -- a structure returned by nf.read('config_name.conf') 
%        field -- is a string of a field name e.g. "Propagator.2.phi"
%        normalize -- is an integer value that determines how the data is
%        scaled.
%                         0 = raw data;
%                         1 = subtract mean of each frame (timepoint);
%                         2 = subtract mean and rescale;
%                         3 = subtract mean, rescale over entire duration.
%        fname -- is optionally a string filename to save the movie
%
% OUTPUT: Generates a movie.
%            -- .
%
% REQUIRES:
%          VideoWriter -- <description>
%          nf.grid -- <description>
%
% AUTHOR:
%     Felix Fung (2012-03-22).
%
% USAGE:
%{
    %
    nf.movie(nf, field, normalize, fname)
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function movie(obj, field, normalize, fname)
    %
    if nargin < 2 || isempty(field)
        field = 'propagator.1.phi';
    end
    
    [data, side] = nf.grid(obj, field);
    [X, Y] = meshgrid(1:side, 1:side);

    datamean = mean(mean(data, 1), 2); % mean of each time point
    
    if nargin < 3 || isempty(normalize)
        normalize = 0;
    end

    plotdata = data;

    % Figure
    fig_handle = figure; 
    fig_handle.PaperUnits = 'inches';
    fig_handle.PaperPosition = [0 0 6 6];
    
    % Main subplot
    h = surf(X, Y, plotdata(:, :, 1)); title('', 'Interpreter', 'none');
    surfplot_handle = gca;
    shading interp; lighting gouraud; camlight;
    % Labels
    xlabel('x', 'fontsize', 18)
    ylabel('y', 'fontsize', 18)
    % Limits
    xlim([1 side])
    ylim([1 side])
    xticks([])
    yticks([])
    colorbar

    
    % Inset
    inset_handle = axes('Parent',fig_handle,'Position',[.58 .6 .3 .3], 'FontSize', 18);
    h2 = surf(X, Y, plotdata(:, :, 1)); title('', 'Interpreter', 'none', 'Parent', inset_handle);
    view(2)
    shading interp;
    % Labels
    xlabel([])
    ylabel([])
    xticks([])
    yticks([])
    % Limits
    xlim([1 side])
    ylim([1 side])
    % Aspect ratio
    axis square

    

    switch normalize
        case 1
            plotdata = bsxfun(@minus, data, datamean);
            set(surfplot_handle, 'CLim', [min(plotdata(:)) max(plotdata(:))])
            set(inset_handle, 'CLim', [min(plotdata(:)) max(plotdata(:))])
        case 2
            for t = 1:obj.npoints
                threshold = zeros(1, obj.npoints);
                norm = zeros(1, obj.npoints);
                threshold(t) = min(min(data(:, :, t)));
                norm(t) = max(max(data(:, :, t) - threshold(t)));
                norm(norm == 0) = 1; % Don't change the normalization if there is no spatial variation
                plotdata(:, :, t) = (data(:, :, t) - threshold(t)) ./ norm(t);
            end
            set(gca, 'CLim', [0 1])
        case 3
            % Choose the normalization based on all the data
            plotdata = bsxfun(@minus, data, datamean);
            plotdata = plotdata ./ max(plotdata(:));
            set(gca, 'CLim', [0 0.1])
    end

    if min(plotdata(:)) == max(plotdata(:)) && abs(min(plotdata(:)) - max(plotdata(:))) < 1.0e-10
        zlim([-1 1]);
    else
        zlim([min(plotdata(:)) max(plotdata(:))]);
    end
    
    colormap(nf.b2r(min(plotdata(:)), max(plotdata(:))))


    F(obj.npoints) = getframe(gcf); % Trick to preallocate F

    for t = 1:obj.npoints
        set(h, 'ZData', plotdata(:, :, t));
        set(surfplot_handle, 'ZLim', [min(plotdata(:)) max(plotdata(:))])
        set(inset_handle, 'ZLim', [min(plotdata(:)) max(plotdata(:))])
        %title(sprintf('%s: Time = %.03f, Mean= %.03f', field, obj.deltat * t, datamean(t)), 'Interpreter', 'none');
        set(h2, 'ZData', plotdata(:, :, t));
        pause(.05);
        if ~(nargin < 4 || isempty(fname))
            F(t) = getframe(gcf);
        end
    end

    % Output a movie if an avi filename was specified
    if ~(nargin < 4 || isempty(fname))
        writerObj = VideoWriter(fname, 'MPEG-4');
        open(writerObj);
        writeVideo(writerObj, F);
        close(writerObj);
    end
end %function movie()
