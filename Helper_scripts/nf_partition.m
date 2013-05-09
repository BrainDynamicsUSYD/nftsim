function r = partition_overlap(n_items,pool,id,frac_overlap)
	% Suppose you have 1:n and want to partition into m groups 
	% Use this function to calculate the boundaries of each group
	% e.g 16 elements with pool=4 means that id=1,2,3,4 returns
	% [1 4] [5 8] [9 12] and [13 16] respectively
    % r = partition(total,id,pool)
    %   total- total number of items (single number) OR a list of the items
    %   pool - total number of partitions
    %   id - the ID of this partition - leave empty to return all the indices
    %   frac_overlap - 
    %   r - start and stop indices, if total is 


    if nargin < 4
        frac_overlap = 0;
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
    
    window_length = n_items/((1+(pool-1)*(1-frac_overlap)));

    window_start = 1:window_length*(1-frac_overlap):window_length*(1-frac_overlap)*pool;
    window_start = floor(window_start);
    window_stop = window_start+floor(window_length)-1;
    window_stop(end) = n_items;

    r = [window_start(:),window_stop(:)];
    if id > 0
        r = r(id,:);
    end



