%% Read a nftsim output file and return a nftsim output struct.
%
% Reads both the text format and the binary formats (nftsim --binary, or
% Format: Binary/Binary32 in the configuration's Output section), detecting
% which from the file itself. The struct is the same for both; binary32
% values are returned as doubles.
%
% A run that was interrupted (or is still running) leaves an incomplete last
% row, which is dropped with a warning.
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
        
        % Physical length of the (first) population's sheet, used by
        % nf.spatial_spectrum; populations share one grid.
        if ~isfield(obj, 'length')
            tok = regexp(buffer, '^\s*Length:\s*([-+0-9.eEdD]+)', 'tokens', 'once');
            if ~isempty(tok)
                obj.length = str2double(tok{1});
            end
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

    ncols = length(obj.fields);
    data_start = ftell(fid);
    meta_line = fgetl(fid);
    if ischar(meta_line) && startsWith(meta_line, 'NFTSIM-BINARY')
        meta = parse_binary_metadata(meta_line);
        data = read_binary_rows(fid, meta, ncols, fname);
    else
        fseek(fid, data_start, 'bof');
        data = read_text_rows(fid, ncols, fname);
    end
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

    if length(obj.time) > 1 || ~exist('meta', 'var')
        obj.deltat = obj.time(2) - obj.time(1);
    else
        obj.deltat = str2double(meta.interval);
    end
    obj.npoints = length(obj.time);
end %function read()


function meta = parse_binary_metadata(line)
    % The key=value pairs of a NFTSIM-BINARY line, as a struct of strings.
    items = strsplit(strtrim(line));
    meta = struct();
    for k = 2:length(items)
        kv = strsplit(items{k}, '=');
        meta.(kv{1}) = kv{2};
    end
    if ~strcmp(meta.version, '1') || ~strcmp(meta.byteorder, 'little') ...
            || ~any(strcmp(meta.dtype, {'float64', 'float32'}))
        error(['nf:' mfilename ':UnsupportedBinary'], ...
              'Unsupported nftsim binary output: %s', line)
    end
end %function parse_binary_metadata()


function data = read_binary_rows(fid, meta, ncols, fname)
    % Rows of a binary output file, as a [rows, ncols] matrix of doubles.
    if str2double(meta.columns) ~= ncols
        error(['nf:' mfilename ':BadBinaryHeader'], ...
              '%s: the header names %d columns, the binary metadata %s.', ...
              fname, ncols, meta.columns)
    end
    if strcmp(meta.dtype, 'float32')
        precision = 'single=>double';
        bytes = 4;
    else
        precision = 'double=>double';
        bytes = 8;
    end
    data_start = ftell(fid);
    values = fread(fid, Inf, precision, 0, 'ieee-le');
    fseek(fid, 0, 'eof');
    nrows = floor(length(values) / ncols);
    partial = ftell(fid) - data_start > nrows * ncols * bytes;
    if strcmp(meta.rows, 'stream')
        % Written to standard output or a pipe, so the count was never known.
        if partial
            warning(['nf:' mfilename ':IncompleteOutput'], ...
                    '%s ends with a partial row: read %d complete rows.', fname, nrows)
        end
    elseif strcmp(meta.rows, 'unknown')
        warning(['nf:' mfilename ':IncompleteOutput'], ...
                '%s is incomplete (the run was interrupted or is still running): read %d complete rows.', ...
                fname, nrows)
    elseif str2double(meta.rows) ~= nrows || partial
        warning(['nf:' mfilename ':IncompleteOutput'], ...
                '%s should have %s rows but holds %d complete rows; read those.', ...
                fname, meta.rows, nrows)
        nrows = min(nrows, str2double(meta.rows));
    end
    data = reshape(values(1:nrows*ncols), ncols, nrows).';
end %function read_binary_rows()


function data = read_text_rows(fid, ncols, fname)
    % Rows of a text output file. A last line without a newline is the
    % incomplete row of an interrupted run, so it is dropped with a warning.
    data = textscan(fid, '%f', 'CollectOutput', true);
    data = data{1};
    fseek(fid, -1, 'eof');
    if fread(fid, 1, 'uint8') ~= 10
        nrows = ceil(length(data) / ncols) - 1;
        warning(['nf:' mfilename ':IncompleteOutput'], ...
                '%s ends with an incomplete row (the run was interrupted or is still running): read %d complete rows.', ...
                fname, nrows)
        data = data(1:nrows*ncols);
    end
    data = reshape(data, ncols, []).';
end %function read_text_rows()


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