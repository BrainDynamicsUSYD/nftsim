function [f,Pnew,Pold,filtered_nf] = nf_spatial_filter(nf,traces,p)
    % [f,P,P_old,filtered_nf] = nf_spatial_filter(nf,traces)
    % Given a grid of voltages, applies a spatial filter   
    % Returns the power spectrum calculated from the centre node of the trace
    % And optionally the spatially smoothed matrix
    % Method from http://www.mathworks.com/matlabcentral/answers/13896-fftshift-of-an-image
    % First, work out the sampling rate in pixels per metre
    data = nf_grid(nf,traces);
    
    % Construct the spatial filter
    v = data(:,:,1);
    Lx = 0.5; % linear cortex dimension (m)
    Ly = 0.5;
    k0 = 10; % spatial filter constant (m^-1)
        
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
    k2_matrix = Kx.^2+Ky.^2;
    
    kmax = 2; % k values to include
    m_rows = -kmax:kmax; % The code will panic and fail if there are not an odd
    n_cols = -kmax:kmax; % number of k values. This ensures the convolution is properly symmetric
    [center_x,center_y] = find(k2_matrix == 0) % Get the centre entry
    
    decimation_factor = 1;%1/200/nf.deltat;
    decimated_rate = 1/nf.deltat/decimation_factor;
    vold = decimate(detrend(data(center_x,center_y,:)),decimation_factor);
    [f,Pold] = pwelch_spectrum(vold,decimated_rate);
    Pnew = zeros(size(Pold));
    
    % FFT to provide V(k,t) - I *think* this is correct? Or may be P(k,t)...
    parfor j = 1:size(data,3)
        output(:,:,j) = fftshift(fft2(data(:,:,j)));
    end
    output = output./2500;

    for m = 0
        for n = 0
            v_temp = decimate(detrend(output(center_x+m,center_y+n,:)),decimation_factor);
            [~,P] = pwelch_spectrum(v_temp,decimated_rate);
            
            k2 = (2*pi*m/Lx)^2 + (2*pi*n/Ly)^2;
            Pnew = Pnew + P.*exp(-k2/k0^2);
        end    
    end    
    
    filtered_nf = nf;
    filtered_nf.data{1} = reshape(shiftdim(output,2),[],2500);
    
    close all
    figure
    loglog(f,Pnew,f,Pold)
    
    if nargin == 3
        [f2,P2] = analytic_spectrum(p,1);
        hold on
        loglog(f2,P2,'r');
        legend('Filtered','Original NF','Analytic');
    end
