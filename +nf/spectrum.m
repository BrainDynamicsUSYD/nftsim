%% Return the frequency and frequency spectrum of given nftsim output.
%
% ARGUMENTS:
%        obj -- A nftsim output struct (a Matlab struct containing data
%               from a simulation).
%        traces -- traces to use
%        n_windows -- number of windows (Default=8).
%
% OUTPUT:
%        f -- Frequency vector.
%        P -- Mean power vector.
%
% REQUIRES:
%        nf.partition() -- Partition a list of n items into m groups.
%        nf.extract() -- Extract a specific subset of data from a nftsim output struct.
%        nf.rfft() -- Fourier transform.
%
% REFERENCES:
%
% AUTHOR:
%     Romesh Abeysuriya (2012-03-22).
%
% USAGE:
%{
    %Load some nftsim output data.
    obj = nf.read('./configs/example.output')
    %Calculate the mean power spectrum.
    [f, P] = nf.spectrum(obj, 'Propagator.1.phi', [], true, true);
    %Plot it.
    figure, plot(f, P)
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%TODO: it would make more sense to return a spectrum for each trace, but that is difficult with the way nf.extract() currently works.

function [f, P] = spectrum(obj, traces, n_windows, windowed, detrended)
    if ~isstruct(obj)
        error(['nf:' mfilename ':BadArgument'], ...
              'The first argument must be a nftsim simulation struct.')
    end

    if nargin < 2 || isempty(traces)
        if ismember('Propagator.1.phi', obj.fields)
            traces = {'Propagator.1.phi'}; % Use 'Propagator.1.phi' by default.
        else
            error(['nf:' mfilename ':BadArgument'], ...
                  'The must specify the trace you want.')
        end
    end
    % if nargin < 2 || isempty(traces)
    %     traces = obj.fields; % Use all fields by default.
    % end

    if nargin < 3 || isempty(n_windows)
        n_windows = 8; % 8 windows by default.
    end

    if nargin < 4 || isempty(windowed)
        windowed = false; % No window function by default.
    end

    if nargin < 5 || isempty(n_windows)
        detrended = false; % Do not detrend by default.
    end

    data = nf.extract(obj, traces);
    fs = 1.0 / obj.deltat;

    % Partition the extracted data into epochs of time.
    frac_overlap = 0.5;
    evenlength = true;
    same_size = true;
    window_idx = nf.partition(size(data, 1), n_windows, [], frac_overlap, evenlength, same_size);
    window_length = window_idx(1, 2) - window_idx(1, 1) + 1;

    % Calculate mean power spectrum for each epoch of time.
    P1 = zeros(size(window_idx, 1), (floor(window_length / 2) + 1));
    for j = 1:size(window_idx, 1)
        [f, ~, P1(j, :)] = nf.rfft(data(window_idx(j, 1):window_idx(j, 2), :), fs, [], windowed, detrended);
    end

    % Mean over time epochs.
    P = mean(P1, 1);

end %function spectrum()