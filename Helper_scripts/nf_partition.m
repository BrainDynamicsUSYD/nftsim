function r = nf_partition(n_items,pool,id,frac_overlap,evenlength,samesize)
	% Suppose you have 1:n and want to partition into m groups 
	% Use this function to calculate the boundaries of each group
	% e.g 16 elements with pool=4 means that id=1,2,3,4 returns
	% [1 4] [5 8] [9 12] and [13 16] respectively
    % r = partition(total,id,pool)
    %   total- total number of items (single number) OR a list of the items
    %   pool - total number of partitions
    %   id - the ID of this partition - leave empty to return all the indices
    %   frac_overlap - amount from 0 to 1 of fractional overlap in windows
    %   evenlength = 1 for the windows to have an even number of elements
    %               This is often necessary if doing an FFT on the window
    %   samesize = 1 for all the windows to be the same size, even if this means skipping elements from the end
    %   r - start and stop indices, if total is 

    if nargin < 6 
        samesize = 0;
    end
    
    if nargin < 5
        evenlength = 0;
    end

    if nargin < 4
        frac_overlap = 0;
    elseif frac_overlap >= 1
        error('frac_overlap must be < 1');
    end

    if nargin < 3 || isempty(id)
        id = 0;
    end

    if nargin < 2 || isempty(pool)
        pool = 1;
    end

    if pool > n_items
        warning('Requested more partitions than there are items')
        pool = n_items;
    end
    
    window_length = floor(n_items/((1+(pool-1)*(1-frac_overlap))));

    if evenlength
        window_length = floor(window_length/2)*2;
    end 

    window_start = floor(1:window_length*(1-frac_overlap):window_length*(1-frac_overlap)*pool);
    window_stop = window_start+window_length-1;
    
    if samesize
        if window_stop(end) > n_items
            error('Error calculating indices- requested an item beyond the end of the array');
        end
    else
        window_stop(end) = n_items;
    end

    r = [window_start(:),window_stop(:)];
    if id > 0
        r = r(id,:);
    end



