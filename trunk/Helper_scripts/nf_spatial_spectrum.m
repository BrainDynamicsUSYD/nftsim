function [f,P,V] = nf_spatial_spectrum(nf,p,kmax,n_windows,spatial_filter)
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
    if isstruct(nf)
        if nargin < 2 || isempty(p)
            if any(strfind([nf.fields{1:end}],'WaveFourier'))
                data = nf_extract(nf,'WaveFourier.1.Real')+1i*nf_extract(nf,'WaveFourier.1.Real');
            else
                p = 'propag.1.phi'; % Try the phi propagator first
                data = nf_grid(nf,p);
            end
        else
            data = nf_grid(nf,p);
        end
        fs = 1/nf.deltat;
    else
        data = nf;
        fs = p;
    end

    if mod(size(data,1),2) || mod(size(data,2),2)
        error('In order to have a zero frequency component, you need an even number of grid edge nodes');
    end
    if nargin < 5 || isempty(spatial_filter)
        spatial_filter = 1; % Spatially filter by default
    end

    if nargin < 4 || isempty(n_windows)
        n_windows = 8; % 8 windows by default
    end

    if nargin < 3 || isempty(kmax)
        kmax = 4; % Limit to 4 k values in each direction
    end

    
    % Calculate the temporal windows
    frac_overlap = 0.5;
    window_vectors = get_window_vectors(size(data,3),n_windows,frac_overlap); 

    % Calculate the Fourier f and k values
    Lx = 0.5; % linear cortex dimension (m)
    Ly = 0.5;
    [f,Kx,Ky] = calculate_fft_components(data(:,:,window_vectors{1}),fs,Lx,Ly);
    
    k2 = Kx.^2+Ky.^2; % Matrix of k-squared values
    if isempty(kmax)
        k_mask = ones(size(k2));
    else
        [center_x,center_y] = find(k2 == 0); % Get the centre entry
        [a,b] = meshgrid(1:size(k2,1),1:size(k2,2));
        k_mask = abs(a-center_x) <= kmax & abs(b-center_y) <= kmax;
    end
    
    if spatial_filter
        % Calculate the value of k^2 at each grid node for spatial filtering
        k0 = 10; % spatial filter constant (m^-1)
        k_filter = exp(-k2./(k0^2));
    else
        k_filter = ones(size(k2));
    end

    P = zeros(size(f));
    for j = 1:length(window_vectors)
        P = P + get_3d_spectrum(data(:,:,window_vectors{j}),k_mask,k_filter,Lx,fs);
    end
    P = P/length(window_vectors);
    
    if nargout > 2
        V = get_filtered_timeseries(data,k_filter,k_mask,k2);
    end
    
function P = get_3d_spectrum(data,k_mask,k_filter,Lx,fs)
    %data = data-mean(data(:));
    %win(1,1,:) = hamming(size(data,3));
    %data = bsxfun(@times,data,win);
    output = fftshift(fftn(data));
    output = output./numel(data);
    
    % Convert to power density
    output = abs(output).^2;
    df = fs/(size(data,3));
    dk = 2*pi/Lx;
    
    %output = output / df / dk / dk; % Get power density in all 3 dimensions
    %output = output*dk*dk; % Since we are summing over k, multiply by dk
    output = output / df; % Take a shortcut and omit converting to density in the spatial direction
    
    % Apply spatial filtering
    output = bsxfun(@times,output,k_mask);
    output = bsxfun(@times,output,k_filter);
    
    % Calculate power spectrum
    P = squeeze(sum(sum(output,1),2)); % A sum is OK here because we have multiplied by dk (so don't need trapz)
    P = ifftshift(P);
    P = P(1:size(data,3)/2+1);
    P(2:size(data,3)/2) = 2*P(2:size(data,3)/2); % Double the frequency components at nonzero and non-Nyquist frequencies

function [f,Kx,Ky] = calculate_fft_components(v,fs,Lx,Ly)
    % Given the 3D matrix of phi, the temporal sampling rate, and the grid dimensions
    % Return the frequencies and wavenumbers that are provided by the FFT
    
    Fs_x = size(v,1)/Lx; % Pixels per *metre* OK
    Fs_y = size(v,2)/Ly;
    
    dx = 1/Fs_x; % Metres per pixel
    dy = 1/Fs_y;
    
    % Vectors for the distance in each direction
    x = dx*(0:size(v,1)-1);
    y = dy*(0:size(v,2)-1);
    
    dFx = Fs_x/size(v,1); % cycles per metre
    dFy = Fs_y/size(v,2);
    
    % Now, construct frequency domain arrays

    Fx = (-Fs_x/2:dFx:Fs_x/2-dFx)'; % cycles per metre
    Fy = (-Fs_y/2:dFy:Fs_y/2-dFy)';
    
    Kx = Fx*2*pi; % wavenumber
    Ky = Fy*2*pi;
    [Kx,Ky] = meshgrid(Kx,Ky);
    f = fs/2*linspace(0,1,size(v,3)/2+1)';

function V = get_filtered_timeseries(data,k_filter,k_mask,k2);
    output = fftshift(fftn(data));
    %output = bsxfun(@times,output,k_mask);
    output = bsxfun(@times,output,k_filter);
    output = ifftn(fftshift(output));
    [a,b] = find(k2==0);
    V = squeeze(output(a,b,:));
    
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
    
    
