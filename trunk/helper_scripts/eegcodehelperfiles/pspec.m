function [P f] = pspec(y,deltat,varargin)
% pspec.m -- ported from diag.pro
%  [P f] = pspec(y,deltat,discard,winlen)

% TODO: make more general


%
% Divide into winlen sec increments which are Fourier transformed to obtain
% power spectra. The power spectra are then averaged to obtain an average
% power spectrum. Note that first 1 sec of data which may have transients
% has to be discarded
%
%
% Calculate number of power spectra and points in each spectrum
%

if nargin>2, % if there is a transient to discard
    discard=varargin{1};
    y=y(1+floor(discard/deltat):end);
end
if nargin>3, % if windowing
    winlen=varargin{2};
    numspectra = floor((length(y)*deltat)/winlen);
    numptsinspectra = floor(winlen/deltat);
else
    numspectra=1;
    numptsinspectra=length(y);
end
    

% if isempty(varargin),  % no windowing or offset
%     numspectra=1;
%     numptsinspectra=length(y);
% else  % window of length winlen (s)
%     winlen=varargin{1};
%     numspectra = floor((length(y)*deltat-1)/winlen); % Note first second is effectively discarded
%     numptsinspectra = floor(winlen/deltat);
% end

%
% Calculate individual spectra and average them
%
if size(y,1)==1,
    powerspectra=zeros(1,numptsinspectra);
else
    if size(y,2)==1,
        powerspectra=zeros(numptsinspectra,1);
    else
        error('time series is not a vector')
    end
end

for i=0:(numspectra-1),
    if exist('winlen','var'),
        %removezero=mean(y(((i+0.5)*numptsinspectra):((i+1+0.5)*numptsinspectra-1)));
        %y(((i+0.5)*numptsinspectra):((i+1+0.5)*numptsinspectra-1),k)=y(((i+0.5)*numptsinspectra):((i+1+0.5)*numptsinspectra-1),k)-removezero;
        %array = abs(fft(y(((i+0.5)*numptsinspectra):((i+1+0.5)*numptsinspectra-1))-removezero)).^2;
        removezero=mean(y((i*numptsinspectra+1):((i+1)*numptsinspectra)));
        array = abs(fft(y((i*numptsinspectra+1):((i+1)*numptsinspectra))-removezero)).^2;
        powerspectra=powerspectra+array;
    else
        powerspectra=abs(fft(y-mean(y))).^2;  % need to have zero mean?
    end
end
powerspectra=powerspectra/numspectra;
%
% Zero out the constant (f=0) point and throw away negative frequencies
% and double the spectrum
%
powerspectra(1)=0; 
powerspectra=2.0*powerspectra(1:(floor(numptsinspectra/2)));  % index should have +1? added floor
%
% Calculate the frequency axis for the power spectrum
%
freq=0:(floor(numptsinspectra/2)-1);  % added floor
freq=freq/(deltat*numptsinspectra);
%
% Plot the power spectrum
%
%
% plot_oo, freq, powerspectra, xrange=[1,2000], yrange=[10E-12,0.01]
% noisefloor=fltarr(numptsinspectra/2)
% noisefloor[*]=powerspectra[1]*10E-7


if nargout==1,
    P=powerspectra;
end
if nargout==2,
    P=powerspectra;
    f=freq;
end