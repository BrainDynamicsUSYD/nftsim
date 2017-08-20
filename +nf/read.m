%% Read a neurofield output file and return a neurofield output struct.
%
% ARGUMENTS:
%        fname -- The name of the neurofield output file to read (absolute
%                 or relative path).
%
% OUTPUT:
%        obj -- A neurofield output struct (a Matlab struct containing data
%               from a simulation).
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
    % The fname argument is required.
    if nargin < 1
        error(['nf:' mfilename ':WrongNumberOfArgs'], ...
              'This function requires a file-name argument.')
    end

    % Check that our input arg is actually a file.
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

    obj.conf_file = fname;

    fid = fopen(fname, 'r'); % Open file for read access

    % Skip through to the start of the output
    buffer = fgetl(fid);
    while isempty(strfind(buffer, '======================='))
        if ~isempty(strfind(buffer, 'Time  |'))
            error(['nf:' mfilename ':OldStyleOutput'], ...
                  'Did you try and open and old-style output file? Found a | that looked like a delimiter.')
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


function [locs, base_index] = get_nodes(line, headers)
    % Given a line with contents like '1 | 1 2 | 1 |
    % return a number of nodes {1,1:2,1}
    % nentries stores the number of entries per line
    nodes = cellfun(@(x) str2double(x), strsplit(line(1:end-2)));
    nodes(1) = 1; % Number of time columns (there is only ever one)
    traces = unique(headers, 'stable');
    locs = cell(1, length(traces));
    base_index = cell(1, length(traces));
    for j = 1:length(traces)
        base_index{j} = strcmp(traces{j}, headers);
        locs{j} = nodes(base_index{j});
    end
end %function get_nodes()
