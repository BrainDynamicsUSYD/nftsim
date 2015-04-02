function [f,P] = spectrum( obj, data )
    % return the frequency and frequency spectrum of given data
    % nf is parsed for information on sampling time
    % data can be a string specifying a trace, or a vector
    
    if strcmp(class(data),'char')
        data = obj.extract(obj,data);
    end
    
%     decimation_factor = (1/nf.deltat)/200; % Downsample to 200Hz
%     if decimation_factor ~= round(decimation_factor) % Unless it is not integer
        decimation_factor = 1;
%     end
%     
    v = decimate(data, decimation_factor );
    [P,f] = pwelch(v, [],[],[],1/obj.deltat/decimation_factor );
    
    figure
    loglog(f,P);
    set(gca,'XLim',[0.1 50]);
    xlabel('Frequency (Hz)');
    ylabel('P(f) Spectral Power Density (arbitrary normalization)');
    title(sprintf('Power spectrum (at %iHz sampling)',1/obj.deltat/decimation_factor));
    
end
