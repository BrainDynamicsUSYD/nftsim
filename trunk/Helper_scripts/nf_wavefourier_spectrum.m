function [f,P] = nf_wavefourier_spectrum(nf,n_windows)
    % [f,P,t,V] = nf_spatial_spectrum(nf,p,kmax,n_windows,spatial_filter)
    % Given a grid of voltages, applies a spatial filter   
    % Returns the spatially summed spectrum
    %  nf - nf object
    %  p - trace to use  
    % ALTERNATE USAGE
    %  nf -> a 3D matrix of values
    %  p - sampling rate
    % 
    % kmax is the number of k-values to use. This should match with whatever 
    %       kmax is set to in the analytic summation
    % n_windows is how many segments to average the time series over
    %       this gives extra spectral clarity at the expense of lower frequency resolution
    % spatial_filter - set to 1 to enable the usual exponential k filter
    %
    
    % First, work out the sampling rate in pixels per metre
    data = nf_extract(nf,'WaveFourier.1.Real')+1i*nf_extract(nf,'WaveFourier.1.Imag');
    fs = 1/nf.deltat;

    if nargin < 2 || isempty(n_windows)
        n_windows = 8; % 8 windows by default
    end


    % Calculate the temporal windows
    frac_overlap = 0.5;
    window_vectors = get_window_vectors(length(data),n_windows,frac_overlap); 

    % Calculate the Fourier f and k values
    f = fs/2*linspace(0,1,length(window_vectors{1})/2+1)';

    P = zeros(size(f));
    for j = 1:length(window_vectors)
        P = P + get_spectrum(data(window_vectors{j}),fs);
    end
    P = P/length(window_vectors);
    
function P = get_spectrum(data,fs)
    %data = data-mean(data(:));
    %win(1,1,:) = hamming(size(data,3));
    %data = bsxfun(@times,data,win);
    P = fftshift(fft(data));
    P = P./numel(data);
    
    % Apply spatial filtering
    %P = bsxfun(@times,P,k_mask);
    %P = bsxfun(@times,P,sqrt(k_filter));
    %keyboard
    % Calculate power spectrum
    %P = squeeze(sum(sum(P,1),2)); % A sum is OK here because we have multiplied by dk (so don't need trapz)

    % Convert to power density
    P = abs(P).^2;
    df = fs/length(data);
    
    %P = P / df / dk / dk; % Get power density in all 3 dimensions
    %P = P*dk*dk; % Since we are summing over k, multiply by dk
    P = P / df; % Take a shortcut and omit converting to density in the spatial direction
    
    P = ifftshift(P);
    P = P(1:length(data)/2+1);
    P(2:length(data)/2) = 2*P(2:length(data)/2); % Double the frequency components at nonzero and non-Nyquist frequencies

function window_vectors = get_window_vectors(l_sample,n_windows,frac_overlap)
    % Return a cell array with indices for each window
    % Sample is calculated from n w[indows overlapping by frac_overlap
    % pwelch uses n_windows = 8, frac_overlap = 0.5 by default
    window_length = l_sample/((1+(n_windows-1)*(1-frac_overlap)));
    % And reduce the window length to a multiple of 2
    window_length = floor(window_length/2)*2;
    
    %window_length = floor(window_length);
    window_start = 1:window_length*(1-frac_overlap):window_length*(1-frac_overlap)*n_windows;
    window_start = floor(window_start);
    window_stop = window_start+floor(window_length)-1;
    if window_stop > l_sample
        error('Error calculating indices!');
    end
    window_vectors = cell(1,n_windows);
    for j = 1:n_windows
        window_vectors{j} = window_start(j):window_stop(j);
    end
    
    
