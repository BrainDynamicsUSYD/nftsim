function [f,s,P] = rfft(y,fs,NFFT,windowed)
    % [f,s,P] = rfft(y,fs,NFFT)
    % y = the time series
    % fs = sampling frequency (Hz) for the time series
    % NFFT = override the amount of zero padding in the FFT
    % windowed = 1 to apply a Hamming window
    % f = frequency values
    % s = Fourier components, denoted c_k in Chris Rennie's notesmean
    % P = spectral power density, correct power is obtained by integrating wrt f
    %
    % Heavily based on Chris's notes http://www.physics.usyd.edu.au/~rennie/fourier.pdf
    
    % The normalization is such that
    % sum(abs(y).^2)/length(y) = trapz(f,P) 
    if nargin < 4 || isempty(windowed)
        windowed = 0;
    end
    
    if nargin < 3 || isempty(NFFT)
        NFFT = length(y); % Use the same length as the input array by default so that normalization is correct
    end

    %y = detrend(y,'constant'); % Remove constant offset and discontinuity at endpoints
    if windowed
        y = y(:).*hamming(length(y));
    end
    
    s = fft(y,NFFT)/(length(y)); % Correctly normalized Fourier components c_k
    %s = ifft(y,NFFT); % Correctly normalized Fourier components c_k with positive exponent in the FFT

    s = s(1:floor(NFFT/2)+1); % This can be returned as the single sided FFT
    P = abs(s).^2; % single sided spectrum 
    if rem(NFFT, 2) % Multiply by 2 to get correct energy, excluding zero and Nyquist frequencies
        % If there is an odd number of points, then there is no frequency component at the Nyquist frequency
        % Therefore, the last frequency component is doubled
        P(2:end) = P(2:end)*2;
    else
        % With an even number of points, there is a Nyquist frequency component which is not duplicated
        % Therefore the last frequency component is NOT doubled
        P(2:end -1) = P(2:end -1)*2;
    end
    
    % P now contains properly normalized spectral power
    %f = fs/2*linspace(0,1,NFFT/2+1)';
    f = 0:fs/NFFT:fs/2; % I think this is more correct
    P = P/f(2); % Divide by frequency bin size to get power density
    
end
