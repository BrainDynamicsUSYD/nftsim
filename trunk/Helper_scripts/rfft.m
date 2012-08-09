function [f,s,P] = rfft(y,fs,NFFT)
    % [f,s,P] = rfft(y,fs,NFFT)
    % y = the time series
    % fs = sampling frequency (Hz) for the time series
    % NFFT = override the amount of zero padding in the FFT
    %
    % f = frequency values
    % s = Fourier components, denoted c_k in Chris Rennie's notesmean
    % P = spectral power density, correct power is obtained by integrating wrt f
    %
    % Heavily based on Chris's notes http://www.physics.usyd.edu.au/~rennie/fourier.pdf
    
    % The normalization is such that
    % sum(abs(y).^2)/length(y) = trapz(f,P) 
    
    if nargin < 3
        NFFT = 2^nextpow2(length(y)); % Next power of 2 from length of y
    end
    NFFT = length(y);
    %y = detrend(y,'constant'); % Remove constant offset and discontinuity at endpoints
    
    s = fft(y,NFFT)/(length(y)); % Correctly normalized Fourier components c_k
    %s = ifft(y,NFFT); % Correctly normalized Fourier components c_k with positive exponent in the FFT

    s = s(1:NFFT/2+1); % This can be returned as the single sided FFT
    P = abs(s).^2; % single sided spectrum 
    if rem(NFFT, 2) % Multiply by 2 to get correct energy, excluding zero and 
    % Nyquist frequencies
        P(2:end) = P(2:end)*2;
    else
        P(2:end -1) = P(2:end -1)*2;
    end
    
    % P now contains properly normalized spectral power
    f = fs/2*linspace(0,1,NFFT/2+1)';
    P = P/f(2); % Divide by frequency bin size to get power density
    
end
