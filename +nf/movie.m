%% Script to generate a movie of a field with spatial variation.
%
% This script assumes the nodes are continuous, and the grid is a square.
%
% ARGUMENTS:
%        obj   -- a structure returned by nf.read('config_name.conf') 
%        field -- is a string of a field name e.g. "Propagator.2.phi"
%        normalize -- is an integer value that determines how the data is
%        scaled.
%                         0 = does nothing - uses the trace specified in `field`;
%                         1 = subtracts mean of each frame (timepoint);
%                         2 = subtracts mean and rescale;
%                         3 = subtracts mean, rescale over entire duration.
%        fname -- is optionally a string filename to save the movie
%
% OUTPUT: Generates a movie and siplays it on screen.
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
    [X, Y] = meshgrid((1:side)*(0.5/side), (1:side)*(0.5/side));
    
    min_x = 0.5/side;
    max_x = 0.5;
        
    min_y = 0.5/side;
    max_y = 0.5;

    datamean = mean(mean(data, 1), 2); % mean of each time point
    
    if nargin < 3 || isempty(normalize)
        normalize = 0;
    end

    plotdata = data;

    % Figure
    fig_handle = figure('pos', [500 500 900 600]); 
    
    % Main subplot
    surf_plot_handle = surf(X, Y, plotdata(:, :, 1)); title('', 'Interpreter', 'none');
    surf_ax_handle = gca;
    surf_ax_handle.FontSize = 20;
    shading interp; lighting gouraud; camlight;
    % Labels
    xlabel('x [m]', 'fontsize', 20)
    ylabel('y [m]', 'fontsize', 20)
    % Limits
    xlim([min_x max_x])
    ylim([min_y max_y])
    %xticks([])
    %yticks([])
    colorbar

    
    % Inset
    inset_ax_handle = axes('Parent',fig_handle,'Position',[.5 .6 .3 .3]);
    inset_plot_handle = surf(X, Y, plotdata(:, :, 1)); title('', 'Interpreter', 'none', 'Parent', inset_ax_handle);
    view(2)
    shading interp;
    % Labels
    xlabel([])
    ylabel([])
    xticks([])
    yticks([])
    % Limits
    xlim([min_x max_x])
    ylim([min_y max_y])
    % Aspect ratio
    axis square
    box on

    

    switch normalize
        case 1
            plotdata = bsxfun(@minus, data, datamean);
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
    
    % Get extrema 
    data_min = min(plotdata(:));
    data_max = max(plotdata(:));
    set(surf_ax_handle, 'CLim', [data_min data_max])
    set(inset_ax_handle, 'CLim', [data_min data_max])
    
    colormap(nf.b2r(data_min, data_max))


    F(obj.npoints) = getframe(gcf); % Trick to preallocate F

    for t = 1:obj.npoints
        set(surf_plot_handle, 'ZData', plotdata(:, :, t));
        title(surf_ax_handle, sprintf('Cortical activity at t= %.03f', obj.deltat * t), 'Interpreter', 'none');
        set(surf_ax_handle, 'ZLim', [data_min data_max])
        %title(sprintf('%s: t= %.03f, Mean at t = %.03f', field, obj.deltat * t, datamean(t)), 'Interpreter', 'none');
        % Only time
        set(inset_ax_handle, 'ZLim',  [data_min data_max])
        set(inset_plot_handle, 'ZData', plotdata(:, :, t));
        pause(.05);
        if ~(nargin < 4 || isempty(fname))
            F(t) = getframe(gcf);
        end
    end

    % Output a movie if an avi filename was specified
    if ~(nargin < 4 || isempty(fname))
        writerObj = VideoWriter(fname);
        open(writerObj);
        writeVideo(writerObj, F);
        close(writerObj);
    end
end %function movie()
