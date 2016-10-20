function [f,P] = spectrum(obj,p,n_windows)    % return the frequency and frequency spectrum of given data
    %  nf - nf object
    %  p - trace to use  

    %  nf -> a 3D matrix of values
    %  p - sampling rate
    if isstruct(obj)
        if nargin < 2 || isempty(p)
            p = 'propagator.1.phi'; % Try the phi propagator first
        end
        data = nf.grid(obj,p);
        fs = 1/obj.deltat;
    else
        data = obj;
        fs = p;
    end

    if nargin < 3 || isempty(n_windows)
        n_windows = 8; % 8 windows by default
    end

    frac_overlap = 0.5;
    window_idx = nf.partition(size(data,2),n_windows,[],frac_overlap,1,1);
    
    for j = 1:length(window_idx)
        [f,~,P1(:,j)] = utils.rfft(data(window_idx(j,1):window_idx(j,2)),fs);
    end

    P = mean(P1,2);


    
end
