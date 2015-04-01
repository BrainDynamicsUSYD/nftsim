function [f,Kx,Ky,x,y,df,dk,dx] = get_frequencies(data,fs,Lx,Ly)
	% Take in the size of the data matrix, the sampling rate, and spatial distances
	% Return corresponding grids of frequencies and positions
	Kx = [];
	Ky = [];
	x = [];
	y = [];

	if isvector(data)
		f = (0:fs/length(data):fs/2).'; % This is the single sided frequency
		return
    end

    f = (0:fs/size(data,3):fs/2).'; % This is the single sided frequency
    df = fs/size(data,3);
    
    Kx = 2*pi*(0:1/Lx:size(data,1)/Lx/2); % This is the single sided frequency
    Ky = 2*pi*(0:1/Ly:size(data,2)/Ly/2); % This is the single sided frequency
    
    if mod(size(data,1),2) % If there is NO nyquist frequency component
    	Kx = [-Kx(end:-1:2) Kx];
    else
    	Kx = [-Kx(end:-1:2) Kx(1:end-1)];
    end

    if mod(size(data,2),2) % If there is NO nyquist frequency component
        Ky = [-Ky(end:-1:2) Ky];
    else
        Ky = [-Ky(end:-1:2) Ky(1:end-1)];
    end
    [Kx,Ky] = meshgrid(Kx,Ky);
    dk = 2*pi/Lx;

    dx = Lx/size(data,1); % Metres per pixel
    x = dx*(0:size(data,1)-1);
    dy = Ly/size(data,1); 
    y = dy*(0:size(data,1)-1);
    [x,y] = meshgrid(x,y);
