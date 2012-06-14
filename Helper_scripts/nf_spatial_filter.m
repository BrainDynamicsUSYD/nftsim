function [f,Pnew,Pold,Pnewloop,filtered_nf] = nf_spatial_filter(nf,traces)
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

    target_node = 12;%floor(size(v,1)/2);
    decimation_factor = 2;%1/200/nf.deltat;
    decimated_rate = 1/nf.deltat/decimation_factor;
    vold = decimate(detrend(data(target_node,target_node,:)),decimation_factor);
    [f,Pold] = pwelch_spectrum(vold,decimated_rate);
    Pnew = zeros(size(Pold));
    
    % Accumulate the power spectrum for each of the k values
    count = 1;
    for m = m_rows
        for n = n_cols
            fprintf('Computing %2i/%2i',count,length(m_rows)*length(n_cols));

            k2 = (2*pi*m/Lx)^2 + (2*pi*n/Ly)^2;
            output = zeros(size(data));
            parfor j = 1:size(data,3)
                vout = fftshift(fft2(data(:,:,j)));
                vout(k2_matrix ~= k2) = 0; % Zero all other k components
                vout = ifft2(ifftshift(vout));
                output(:,:,j) = abs(vout);
            end
            
            vnew = decimate(detrend(output(target_node,target_node+1,:)),decimation_factor);
            [~,P] = pwelch_spectrum(vnew,decimated_rate);

            Pnew = Pnew + P.*exp(-k2/k0^2);
            fprintf('\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b');
            count = count+1;
        end
    end
    
    k0filter = exp(-(Kx.^2+Ky.^2)/k0^2);
    parfor j = 1:size(data,3)
        vout = fftshift(fft2(data(:,:,j)));
        vout = vout.*k0filter;
        vout = ifft2(ifftshift(vout));
        output(:,:,j) = abs(vout);
    end
    
    vnew = decimate(detrend(output(target_node,target_node,:)),decimation_factor);
    [f,Pnewloop] = pwelch_spectrum(vnew,1/nf.deltat/decimation_factor);
    
    
    filtered_nf = nf;
    filtered_nf.data{1} = reshape(shiftdim(output,2),[],2500);

