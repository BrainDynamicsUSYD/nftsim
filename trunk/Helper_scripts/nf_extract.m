function data = nf_extract(nf,traces,t,nodes)
    % output = nf_extract(nf,traces,t,nodes)
    % Given an nf object, return data from certain traces at particular times
    % nf - nf object
    % t - can be a time e.g. 0.001 or a range [0.001 0.1]. If empty, returns all times
    % traces - comma separated list of traces e.g. 'Propag.2.phi, Couple.2.nu'
    % nodes - an array of node numbers that are requested e.g. [1 2 3]
    % traces and nodes are optional
    
    % Romesh Abeysuriya 120322
    
    % If no nodes are provided, output all nodes
    if nargin < 4 || isempty(nodes)
        nodes = nf.nodes{1}; 
    elseif ~all(ismember(nodes,nf.nodes{1}))
        error('You have requested a nonexistant node');
    end
    
    % If no time is provided, output all times
    if nargin < 3 || isempty(t) 
        start = 1;
        stop = length(nf.time);
    else
        % Find the start/stop times
        % These commands return an empty matrix if requested times are out of range
        start = find(nf.time<=t(1),1,'last'); 
        if length(t) == 2
            stop = find(nf.time>=t(end),1,'first');
        else
            stop = start;
        end
        
        if isempty(start) || isempty(stop)
            error('Requested time(s) are out of range');
        end
    end
    
    % If no traces are requested, output all traces
    if nargin < 2 || isempty(traces)
        outputs = 1:length(nf.fields);
    else
        traces = regexp(traces,' *, *','split'); % Split traces into cell array
        outputs = [];
        for j = 1:length(traces)
            idx = find(strcmpi(traces{j},nf.fields));
            if isempty(idx)
                error(sprintf('Requested trace "%s" could not be found',traces{j}));
            else
                outputs(end+1) = idx;
            end
        end
    end
    
    % And assemble the output
    data = [];
    for j = 1:length(outputs)
        data = [data nf.data{outputs(j)}(start:stop,nodes)];
    end
    
    
