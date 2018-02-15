%% Read a nftsim output file and return a nftsim output struct.
%
% ARGUMENTS:
%        fname -- The name of the nftsim output file to read (absolute
%                 or relative path).
%
% OUTPUT:
%        obj -- A nftsim output struct. A Matlab struct containing data
%               and parameters from a simulation.
%
% AUTHOR:
%     Romesh Abeysuriya (2012-03-22).
%
% USAGE:
%{
    %
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [obj] = read(fname)

    % Check that our input arg is actually a file.
    if endsWith(fname,'.conf')
        error(['nf:' mfilename ':BadArgument'], ...
                  'The file provided is a configuration file: "%s".', fname)
    end
    if ~exist(fname, 'file') 
        % Try appending .output to the name we were provided.
        if ~exist([fname, '.output'], 'file')
            error(['nf:' mfilename ':FileDoesNotExist'], ...
                  'The output file provided does not exist: "%s".', fname)
        else % The argument we were provided was just missing a suffix.
            % Append the .output suffix.
            fname = [fname, '.output'];
        end
    end

    obj.output_file = fname;

    fid = fopen(fname, 'r'); % Open file for read access

    % Skip through to the start of the output
    buffer = fgetl(fid);
    
    
    
        
    while isempty(strfind(buffer, '======================='))
        % TODO: CLEAN UP - this part refers to EEGCODE.
        if ~isempty(strfind(buffer, 'Time  |'))
            error(['nf:' mfilename ':OldStyleOutput'], ...
                  'Did you try and open and old-style output file? Found a | that looked like a delimiter.')
        end
        
        
        key   = 'Nodes'; 
        if  ~isempty(strfind(buffer, key))
            idx = strfind(buffer, key, 'ForceCellOutput', true);
            obj.input_nodes = sscanf(buffer(idx{1}(1) + length(key)+2:end), '%d', 1);
        end
        
        key   = 'nodes'; 
        if  ~isempty(strfind(buffer, key))
            idx = strfind(buffer, key, 'ForceCellOutput', true);
            obj.longside_nodes = sscanf(buffer(idx{1}(1) + length(key)+2:end), '%d', 1);
            if isempty(obj.longside_nodes)
                obj = rmfield(obj, 'longside_nodes');
            end
        end       

        buffer = fgetl(fid);
        
    end
    fgetl(fid); % Skip the empty line following the '===============' barrier.

    headers = fgetl(fid);
    nodes = fgetl(fid);

    obj.fields = strsplit(headers);
    obj.fields = obj.fields(2:end-1);

    [obj.nodes, base_index] = get_nodes(nodes, obj.fields);

    data = textscan(fid, '%f', 'CollectOutput', true);
    data = reshape(data{1}, length(obj.fields), []).';

    for j = 1:length(obj.nodes)
        obj.data{j} = data(:, base_index{j});
    end
    clear data
    fclose(fid);

    % Finally, move the time to an array of its own
    obj.time = obj.data{1};
    obj.data = obj.data(2:end);
    obj.fields = unique(obj.fields(2:end), 'stable');
    obj.nodes = obj.nodes(2:end);

    obj.deltat = obj.time(2) - obj.time(1);
    obj.npoints = length(obj.time);
end %function read()


function [locs, base_index] = get_nodes(this_line, headers)
    % Given a line with contents like '1 | 1 2 | 1 |
    % return a number of nodes {1,1:2,1}
    % nentries stores the number of entries per line
    nodes = cellfun(@(x) str2double(x), strsplit(this_line(1:end-2)));
    nodes(1) = 1; % Number of time columns (there is only ever one)
    traces = unique(headers, 'stable');
    locs = cell(1, length(traces));
    base_index = cell(1, length(traces));
    for j = 1:length(traces)
        base_index{j} = strcmp(traces{j}, headers);
        locs{j} = nodes(base_index{j});
    end
end %function get_nodes()