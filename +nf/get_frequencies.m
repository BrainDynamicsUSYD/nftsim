%% Take in the size of the data matrix, the sampling rate, and spatial distances
% Return corresponding grids of frequencies and positions
%
% ARGUMENTS:
%        data -- an 1D or 3D array of size space points along x, space points along y, tim points, 
%        fs -- sampling temporal frequency in [Hz]
%        Lx -- physical size of the spatial domain along x in [m] 
%        Ly -- physical size of the spatial domain along y in [m]
%
% OUTPUT:
%        f -- vector of (positives) frequencies in [Hz]
%        Kx -- vector of angular wavenumbers in [rad/m]
%        Ky -- vector of angular wavenumbers in [rad/m]
%        x -- vector with spatial coordinates in [m]
%        y -- vector with spatial coordinates in [m]
%        df -- temporal frequency resolution in [Hz]
%        dk -- radial spatial frequency resolution in [rad/m]
%        dx -- spatial resolution in [m]
%
% REQUIRES:
%           -- <description>
%           -- <description>
%
% REFERENCES:
%
% AUTHOR:
%     Romesh Abeysuriya (2012-03-22).
%
% USAGE:
%{
    %
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [f, Kx, Ky, x, y, df, dk, dx] = get_frequencies(data, fs, Lx, Ly)
    Kx = [];
    Ky = [];
    x = [];
    y = [];

    % This is the single positive sided frequency vector
    if isvector(data)
        f = (0:(fs / length(data)):(fs / 2)).'; 
        return
    end

    % This is the single positive sided frequency
    f = (0:(fs / size(data, 3)):(fs / 2)).'; 
    df = fs / size(data, 3);

    % Wavenumbers / single sided and positive
    Kx = 2*pi*(0:1/Lx:size(data, 1)/Lx/2);
    Ky = 2*pi*(0:1/Ly:size(data, 2)/Ly/2);
    
    if mod(size(data, 1), 2) % If there is NO nyquist frequency component
        Kx = [-Kx(end:-1:2) Kx];
    else
        Kx = [-Kx(end:-1:2) Kx(1:end-1)];
    end

    if mod(size(data, 2), 2) % If there is NO nyquist frequency component
        Ky = [-Ky(end:-1:2) Ky];
    else
        Ky = [-Ky(end:-1:2) Ky(1:end-1)];
    end
    
    [Kx, Ky] = meshgrid(Kx, Ky);
    
    % Smallest wavenumber - resolution in k-space (angular spatial frequency)
    % This is assuming the spatial domain is square (Lx=Ly)
    
    dk = 2 * pi / Lx;
    
    % dx and dy should be the same
    dx = Lx / size(data, 1); % [m]
    dy = Ly / size(data, 1); % [m]
    
    % the stencil in NFTsim assumes the coordinates of a parcel of the discretized domain
    % is at the centre of the parcel, thus the first coordinate is at (dx/2; dy/2) 

    x  = dx * (0:size(data, 1)) + dx/2;
    y  = dy * (0:size(data, 1)) + dx/2;

    [x, y] = meshgrid(x, y);

end %function get_frequencies()
