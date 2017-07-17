%% Return the frequency and frequency spectrum of given data.
%
% ARGUMENTS:
%        obj -- nf object
%        p -- trace to use
%        n_windows -- number of windows (Default=8).
%
% OUTPUT:
%        f -- .
%        P -- .
%
% REQUIRES:
%        nf.partition()  -- <description>
%        nf.grid() -- <description>
%        nf.rfft() -- <description>
%
% REFERENCES:
%
% AUTHOR:
%     Romesh Abeysuriya (2012-03-22).
%
% USAGE:
%{
    %
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [f, P] = spectrum(obj, p, n_windows)
    if isstruct(obj)
        if nargin < 2 || isempty(p)
            p = 'propagator.1.phi'; % Try the phi propagator first
        end
        data = nf.grid(obj, p);
        fs = 1.0 / obj.deltat;
    else
        data = obj;
        fs = p;
    end

    if nargin < 3 || isempty(n_windows)
        n_windows = 8; % 8 windows by default
    end

    frac_overlap = 0.5;
    window_idx = nf.partition(size(data, 2), n_windows, [], frac_overlap, 1, 1);
    
    for j = 1:length(window_idx)
        [f, ~, P1(:, j)] = nf.rfft(data(window_idx(j, 1):window_idx(j, 2)), fs);
    end

    P = mean(P1, 2);

end %function spectrum()
