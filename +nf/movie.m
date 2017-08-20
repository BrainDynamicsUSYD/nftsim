%% Script to generate a movie of a field with spatial variation.
%
% This script assumes the nodes are continuous, and the grid is a square.
%
% ARGUMENTS:
%        obj -- .
%        field -- is a string of a field name e.g. "Propagator.2.phi"
%        normalize -- is an integer value:
%                         0 = raw data;
%                         1 = subtract mean;
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

    datamean = mean(mean(data, 1), 2);
    
    if nargin < 3 || isempty(normalize)
        normalize = 0;
    end

    plotdata = data;

    figure; h = surf(X, Y, plotdata(:, :, 1)); title('', 'Interpreter', 'none');

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
    xlabel('x')
    ylable('y')

    shading interp; lighting gouraud; camlight;
    F(obj.npoints) = getframe(gcf); % Trick to preallocate F

    for t = 1:obj.npoints
        set(h, 'ZData', plotdata(:, :, t));
        title(sprintf('%s: Time = %.03f, Mean= %.03f', field, obj.deltat * t, datamean(t)), 'Interpreter', 'none');
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
