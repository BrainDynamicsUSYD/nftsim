function [f,P,Pold] = nf_spatial_filter(nf,traces,p)
    % [f,P,P_old,filtered_nf] = nf_spatial_filter(nf,traces)
    % Given a grid of voltages, applies a spatial filter   
    % Returns the power spectrum calculated from the centre node of the trace
    % And optionally the spatially smoothed matrix
    % Method from http://www.mathworks.com/matlabcentral/answers/13896-fftshift-of-an-image
    
    % First, work out the sampling rate in pixels per metre
    if strcmp(class(nf),'struct')
        data = nf_grid(nf,traces);
        fs = 1/nf.deltat;
    else
        data = nf;
        fs = traces;
    end
        
    n_windows = 8; % like pwelch, 8 windows, 50% overlap
    frac_overlap = 0.5;
    window_vectors = get_window_vectors(size(data,3),n_windows,frac_overlap); 

    % Calculate the Fourier f and k values
    Lx = 0.5; % linear cortex dimension (m)
    Ly = 0.5;
    [f,Kx,Ky] = calculate_fft_components(data(:,:,window_vectors{1}),fs,Lx,Ly);

    k0 = 10; % spatial filter constant (m^-1)
    k2 = Kx.^2+Ky.^2; % Matrix of k-squared values
    k_filter = exp(-k2./(k0^2));
    
    kmax = 2; % k values to include
    [center_x,center_y] = find(k2 == 0); % Get the centre entry
    [a,b] = meshgrid(1:size(k2,1),1:size(k2,2));
    k_mask = abs(a-center_x) <= kmax & abs(b-center_y) <= kmax;
    [fold,~,Pold] = rfft(squeeze(data(center_x,center_y,:)),fs);
    
    P = zeros(size(f));
    for j = 1:length(window_vectors)
        P = P + get_3d_spectrum(data(:,:,window_vectors{j}),k_mask,k_filter,Lx,fs);
    end
    P = P/length(window_vectors);
    P = P*100;
    %return
    
    %close all
    figure
    loglog(f,P,fold,Pold)
    %loglog(f,P);
    
    if nargin == 3
        [f2,P2] = analytic_spectrum(p,1);
        hold on
        loglog(f2,P2,'r');
        legend('Filtered','Original NF','Analytic');
    end

function P = get_3d_spectrum(data,k_mask,k_filter,Lx,fs)
    data = data - mean(data(:));
    win(1,1,:) = hamming(size(data,3));
    data = bsxfun(@times,data,win);
    output = fftshift(fftn(data));
    output = output./numel(data);
    
    % Convert to power, and filter
    output = abs(output).^2;
    % Convert to energy
    
    output = output * Lx * Lx * size(data,3) / fs;

    %output = output*0.001;
    output = bsxfun(@times,output,k_mask);
    output = bsxfun(@times,output,k_filter);

    %output = output * Lx * Lx * size(data,3) / fs;
    output = output / Lx / Lx; % Account for integration by summation. 
    P = squeeze(sum(sum(output,1),2));
    P = ifftshift(P);
    P = 2*P(1:size(data,3)/2+1);
    % Todo: exclude zero and nyquist frequency parts from this multiplication
    
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

function window_vectors = get_window_vectors(l_sample,n_windows,frac_overlap)
    % Return a cell array with indices for each window
    % Sample is calculated from n windows overlapping by frac_overlap
    % pwelch uses n_windows = 8, frac_overlap = 0.5 by default
    window_length = l_sample/((1+(n_windows-1)*(1-frac_overlap)));
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
    
    
