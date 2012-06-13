function [f,P,Pold,filtered_nf] = nf_spatial_filter(nf,traces)
    % [f,P,filtered_nf] = nf_spatial_filter(nf,traces)
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
        
    Fs_x = size(v,1)/Lx; % Pixels per metre
    Fs_y = size(v,2)/Ly;
    
    dx = Lx/Fs_x; % Metres per pixel
    dy = Ly/Fs_y;
    
    % Vectors for the distance in each direction
    x = dx*(0:size(v,1)-1);
    y = dy*(0:size(v,2)-1);
    
    dFx = Fs_x/size(v,1); % cycles per cm increments
    dFy = Fs_y/size(v,2);
    
    % Now, construct frequency domain arrays

    Fx = (-Fs_x/2:dFx:Fs_x/2-dFx)'; % cycles per metre
    Fy = (-Fs_y/2:dFy:Fs_y/2-dFy)';
    
    Kx = Fx*2*pi; % wavenumber
    Ky = Fy*2*pi;
    [Kx,Ky] = meshgrid(Kx,Ky);
    

    k0filter = exp(-(Kx.^2+Ky.^2)/k0^2);
    
    % Now we have the spatial filter. Go through and apply it
    parfor j = 1:size(data,3)
        vout = fftshift(fft2(data(:,:,j)));
        vout = vout.*k0filter;
        vout = ifft2(ifftshift(vout));
        output(:,:,j) = abs(vout);
    end
    
    decimation_factor = 2;%1/200/nf.deltat;
    center_node = floor(size(v,1));
    vnew = decimate(detrend(output(center_node,center_node,:)),decimation_factor);
    vold = decimate(detrend(data(center_node,center_node,:)),decimation_factor);

    [f,P] = pwelch_spectrum(vnew,1/nf.deltat/decimation_factor);
    [f,Pold] = pwelch_spectrum(vold,1/nf.deltat/decimation_factor);
    filtered = reshape(shiftdim(output,2),[],2500);
    filtered_nf = nf;
    filtered_nf.data{1} = filtered;
    
    
    subplot(1,3,1)
    surf(x,y,data(:,:,end));
    subplot(1,3,2)
    surf(Kx,Ky,k0filter);
    subplot(1,3,3)
    surf(x,y,output(:,:,end));
    
