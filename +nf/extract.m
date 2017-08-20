%% Extract a specific subset of data from a neurofield output struct.
%
% The subset can be specified in terms of: traces; times; and nodes.
%
% ARGUMENTS:
%        obj -- A neurofield output struct (a Matlab struct containing data
%               from a simulation).
%        traces -- A cell array or a comma separated string of the traces
%                  you want to extract, e.g. 'Propagator.2.phi, Coupling.2.nu'.
%        t -- Time of interest, can be a single time, e.g. 0.001, or a range
%             of time, e.g. [0.001 0.1]. If empty, all times are returned.
%        nodes -- An array of node numbers that are requested e.g. [1 2 3].
%
% OUTPUT:
%        data -- (tpts, N).
%        t2 -- .
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
    output = nf.extract(obj, traces, t, nodes)
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [data, t2] = extract(obj, traces, t, nodes)
    % If no nodes are provided, output all nodes
    if ~isstruct(obj) || ~isfield(obj, 'data')
        error(['nf:' mfilename ':BadArgument'], ...
              'The first argument must be a neurofield output struct.');
    end
    
    if nargin < 4 || isempty(nodes)
        nodes = obj.nodes{1}; 
    elseif ~all(ismember(nodes, obj.nodes{1}))
        error(['nf:' mfilename ':NonexistentNode'], ...
              'You have requested a non-existent node.');
    end

    % If no time is provided, output all times
    if nargin < 3 || isempty(t)
        start = 1;
        stop = length(obj.time);
    else
        % Check boundaries
        if length(t) > 1
            if t(1) > t(2)
                fprintf('Start time larger than stop time- reversing order\n');
                t = t([2, 1]);
            end

            if t(2) > obj.time(end)
                fprintf('End time out of bounds- adjusting %.1fs to %.1fs\n', t(2), obj.time(end));
                t(2) = obj.time(end);
            end
        end %length(t) > 1
        
        if t(1) < obj.time(1)
            fprintf('Start time out of bounds- adjusting %.1fs to %.1fs\n', t(1), obj.time(1));
            t(1) = obj.time(1);
        end

        % Find the start/stop times
        % These commands return an empty matrix if requested times are out of range
        start = find(obj.time <= t(1), 1, 'last');
        if length(t) == 2
            stop = find(obj.time >= t(end), 1, 'first');
        else
            stop = start;
        end
        
        if isempty(start) || isempty(stop)
            error('Could not find times');
        end
    end

    % If no traces are requested, output all traces
    if nargin < 2 || isempty(traces)
        outputs = 1:length(obj.fields);
    else
        if ischar(traces)
            traces = regexp(traces, ' *, *', 'split'); % Split traces into cell array
        end
        if ~iscell(traces)
            error(['nf:' mfilename ':BadArgument'], ...
                  'The traces arg must be a string or cell array of strings.');
        end
        outputs = zeros(1, length(traces));
        for j = 1:length(traces)
            idx = find(strcmpi(traces{j}, obj.fields));
            if isempty(idx)
                error(['nf:' mfilename ':UnrecognisedTrace'], ...
                      'Requested trace "%s" could not be found', traces{j});
            else
                outputs(j) = idx;
            end
        end
    end

    % Gather requested traces, nodes, and time-points into a single array (tpts, N).
    n = length(nodes);
    data = zeros(stop-start+1, length(outputs) * n);
    for j = 1:length(outputs)
        [~, node_index] = ismember(nodes, obj.nodes{j});
        data(:, (((j - 1) * n) + 1):(j * n)) = obj.data{outputs(j)}(start:stop, node_index);
    end
    t2 = obj.time(start:stop);

end %function extract()
