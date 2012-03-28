function [f,s] = pwelch_spectrum(y,fs)
    % This function calculates the power spectrum for a given time series
    % and sampling rate. View the spectrum with loglog(f,s)
    window_length = 1; % Window length in seconds

    [s,f] = pwelch(y,[],[],[],fs);
    
    high = 45; low = 1; % Same truncation frequencies as get_tfs()
	s = s(1:find(f<=high, 1, 'last' )); % truncate to < high Hz
	f = f(1:find(f<=high, 1, 'last' ));
    s = s(find(f>=low, 1, 'first' ):end); % truncate to > low Hz
	f = f(find(f>=low, 1, 'first' ):end);
	%s = s./mex_trapz(f,s);
end

