function [cfx, cfreqso] = wavelet_spectrogram(obj, traces, x, fs, cfreqs, morletParam, plotFlag)
% Takes the complex Morlet wavelet transform of time series data and plots
% spectrogram
% 
% ARGUMENTS:
%       x -- 1xT vector time series or NxT matrix of time series recorded
%            across N channels
%       fs -- Sampling frequency (in Hz)
%       cfreqs -- Vector of centre frequencies
%       morletParam -- (Optional) Morlet wavelet parameter, allows trade off
%           between time and frequency resolution (higher is better
%           frequency resolution). Default value 7
%       plotFlag -- (Optional) morletWaveletTransform(..., 'plot') means
%          function will create time-frequency plots, with or without
%          morletParam specified. Plots will be averaged across channels.
%          Default is no plotting
%
% OUTPUTS:
%       cfs -- FxNxT matrix of complex Morlet wavelet coefficients, where F
%           is the number of centre frequencies
%       cfreqso -- Vector of centre frequencies. Should always be equal to
%           input argument cfreqs, but this may not be true in every
%           situation.
%
% USAGE:
%{
        fs = 1000;
        t = 0:(1/Fs):2;
        x = chirp(t,3,1,8,'quadratic');
        cfreqs = linspace(1, 10, 100);
        % Use built-in spectrogram plotting with default Morlet parameter
        wavelet_spectrogram(x, fs, cfreqs, 'plot');
        % Manually plot spectrogram with smaller parameters
        [cfx, cfreqs] = wavelet_spectrogram(x, fs, cfreqs, 3);
        figure
        imagesc(t, cfreqs, abs(cfx))
        axis xy
%}
%
% Rory Townsend, Oct 2016
% rory.townsend@sydney.edu.au

if ~isstruct(obj)
        error(['nf:' mfilename ':BadInputArgument'], ...
              'The first argument must be a struct as returned by nf.read().')
    end

    if nargin < 2 || isempty(traces)
        if ismember('Propagator.1.phi', obj.fields)
            traces = {'Propagator.1.phi'}; % Use 'Propagator.1.phi' by default.
        else
            error(['nf:' mfilename ':BadInputArgument'], ...
                  'You need to specify the name of a trace (eg, Propagator.1.phi).')
        end
    end


% Set up paramters for morlet wavelet transform
if ~exist('morletParam', 'var') || strcmp(morletParam, 'plot')
    if strcmp(morletParam, 'plot')
        plotFlag = 'plot';
    end
    morletParam = 7;
end
dt = 1/fs;
morletFourierFactor = 4*pi/(morletParam+sqrt(2+morletParam^2));

% Set up a larger output matrix if X has multiple channels / trials
if ~isvector(x)
    cfx = zeros([length(cfreqs), size(x)]);
end

% Old code using CWT rather than CWTFT
% wname = 'morl';
% scales = centfrq(wname)./(cfreqs*dt);

% Set up structure defining scales between min and max pseudo-frequencies
scales = 1./(morletFourierFactor * cfreqs);

% Calculate wavelet coefficients for each channel
if ~isvector(x)
    for ichan = 1:size(x,1)
        %icfs = cwt(x(ichan,:), scales, wname);
        cfstruct = cwtft({x(ichan,:),dt},'scales',scales,'wavelet',{'morl', morletParam});
        cfx(:, ichan, :) = permute(cfstruct.cfs, [1, 3, 2]);
    end
    plotVal = squeeze(mean(cfx, 2));
else
    %cfs = cwt(x, scales, wname);
    cfstruct = cwtft({x,dt},'scales',scales,'wavelet',{'morl',morletParam});
    cfx = cfstruct.cfs;
    plotVal = cfx;
end

sc = cfstruct.scales;
cfreqso = 1./(sc*morletFourierFactor);

% Plot data 
if exist('plotFlag', 'var') && (strcmp(plotFlag, 'plot'))
    figure
    % Generate time-frequency power plot
    time = (1:length(x))/fs;
    imagesc(time, cfreqso, abs(plotVal))
    xlabel('Time (s)')
    ylabel('Frequency (Hz)')
    title('Morlet wavelet power')
    axis xy
    
    % Generate time-frequency phase plot
    figure
    imagesc(time, cfreqso, angle(plotVal))
    % Note: hsv is MATLAB's only built-in colormap suitable for circular
    % data, but it is very ugly
    if exist('pmkmp_new', 'file') == 2
        colormap(pmkmp_new(256, 'ostwald_o'));
    else
        colormap(hsv)
    end
    xlabel('Time (s)')
    ylabel('Frequency (Hz)')
    title('Morlet wavelet phase')
    axis xy
end