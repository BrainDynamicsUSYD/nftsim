%% Partition a list of n items into m groups.
%
% This function calculates the boundaries of each group e.g 16 elements
% with pool=4 means that id=1,2,3,4 returns [1 4] [5 8] [9 12] and [13 16]
% respectively.
%
% ARGUMENTS:
%        n_items -- .
%        pool -- total number of partitions
%        id -- the ID of this partition - leave empty to return all the indices
%        frac_overlap -- amount from 0 to 1 of fractional overlap in windows
%        evenlength -- Set as true for the windows to have an even number
%                      of elements. This is often necessary if doing an FFT
%                      on the window.
%        samesize -- Set as true for all the windows to be the same size,
%                    even if this means skipping elements from the end
%
% OUTPUT:
%        r -- start and stop indices, if total is
%
% AUTHOR:
%     Romesh Abeysuriya (2012-03-22).
%
% USAGE:
%{
    %
    r = nf.partition(n_items,pool,id,frac_overlap,evenlength,samesize)
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%TODO: if we're using this for time, it'd be useful to support partition length
%      as an alternative to number of partitions (pool).

function r = partition(n_items, pool, id, frac_overlap, evenlength, samesize)
    %
    if nargin < 6
        samesize = false;
    end
    
    if nargin < 5
        evenlength = false;
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
    
    window_length = floor(n_items / ((1 + (pool - 1) * (1 - frac_overlap))));

    if evenlength
        window_length = floor(window_length / 2) * 2;
    end 

    window_start = floor(1:window_length*(1-frac_overlap):window_length*(1-frac_overlap)*pool);
    window_stop = window_start + window_length - 1;
    
    if samesize
        if window_stop(end) > n_items
            error('Error calculating indices- requested an item beyond the end of the array');
        end
    else
        window_stop(end) = n_items;
    end

    r = [window_start(:), window_stop(:)];
    if id > 0
        r = r(id, :);
    end
end %function r = partition()
