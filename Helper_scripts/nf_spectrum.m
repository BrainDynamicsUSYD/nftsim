function [f,P] = nf_spectrum( nf, data )
    % return the frequency and frequency spectrum of given data
    % nf is parsed for information on sampling time
    decimation_factor = 1;
    v = decimate( detrend(data), decimation_factor );
    [f,P] = pwelch_spectrum( v, 1/nf.deltat/decimation_factor );
end
