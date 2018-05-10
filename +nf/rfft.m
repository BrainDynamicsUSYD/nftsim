%% Fourier transform
%
% Heavily based on Chris Rennie's notes, see REFERENCES below.
%
% The normalization is such that sum(abs(y).^2)/length(y) = trapz(f,P)
%
% ARGUMENTS:
%        y  -- a 2D array of size (timepoints x N), where N is the number of signals/timeseries or traces.
%        fs -- sampling frequency (Hz) of the time series.
%        NFFT -- override the amount of zero padding in the FFT.
%        windowed  -- set as true to apply a Hamming window.
%        detrended -- set as true to remove any constant offset and linear trend.
%
% OUTPUT:
%        f -- frequency values
%        Y -- Fourier components, denoted c_k in Chris Rennie's notes.
%        P -- (fpts, N) Mean spectral power density, correct power is obtained by
%             integrating w.r.t. frequency values (f).
%
% REFERENCES:
%        http://www.brain-dynamics.net/~chris_rennie/fourier.pdf
%
% AUTHOR:
%     Original: Chris Rennie circa 2000
%     Romesh Abeysuriya (2012-03-22).
%
% USAGE:
%{
    %
    [f, Y, P] = nf.rfft(y, fs, NFFT)
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [f, Y, P] = rfft(y, fs, NFFT, windowed, detrended, one_sided)
    %
    if nargin < 6
       one_sided = true;
    end

    if nargin < 5 || isempty(detrended)
        detrended = false;
    end

    if nargin < 4 || isempty(windowed)
        windowed = false;
    end
    
    if nargin < 3 || isempty(NFFT)
        NFFT = size(y, 1); % Use the same length as the input array by default so that normalization is correct
    end

    %For a single time-series we need size (tpts, 1)
    if size(y, 1) == 1
        y = y.';
    end

    % Remove constant offset and linear trend
    if detrended
        y = detrend(y);
    end

    % Remove discontinuity at endpoints.
    if windowed
        window_func = repmat(hamming(size(y, 1)), [1, size(y, 2)]);
        y = y .* window_func;
    end
    
    Y = fft(y, NFFT) / size(y, 1); % Correctly normalized Fourier components c_k
    %s = ifft(y, NFFT); % Correctly normalized Fourier components c_k with positive exponent in the FFT



    freq_bin_width = fs / NFFT;

    if one_sided
        % frequency vector
        f = 0:freq_bin_width:(fs/2); 
        Y = Y(1:(floor(NFFT / 2) + 1), :); % This can be returned as the single sided amplitude spectrum
        P = abs(Y).^2; % single sided spectrum 

        if rem(NFFT, 2) % Multiply by 2 to get correct energy, excluding zero and Nyquist frequencies
            % If there is an odd number of points, then there is no frequency component at the Nyquist frequency
            % Therefore, the last frequency component is doubled
            P(2:end, :) = P(2:end, :) .* 2.0;
        else
            % With an even number of points, there is a Nyquist frequency component which is not duplicated
            % Therefore the last frequency component is NOT doubled
            P(2:(end - 1), :) = P(2:(end - 1), :) .* 2.0;
        end
            
    else
        Y = fftshift(Y);
        f = -(fs/2):freq_bin_width:(fs/2)-freq_bin_width;
        P = abs(Y).^2; % two sided spectrum 

    end
        P = mean(P, 2);
        P = P ./ freq_bin_width; % Divide by frequency bin size to get power spectral density

end %function rfft()
