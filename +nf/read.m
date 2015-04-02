function nf = read(fname,himem);
    % nf_read = read_nf(fname);
    % himem = 1 uses slower method that can avoid out of memory errors in some cases
    % Read a neurofield output file and return a neurofield output struct
    % for use with other helper scripts
    % Romesh Abeysuriya 120322
    if nargin < 2 || isempty(himem)
        himem = 0;
    end


    if ~exist(fname,'file')
        fname = [fname,'.output'];
    end
    
    % if himem
    %     nrows = count_lines(fname) - 2;
    % end

    fid = fopen(fname,'r'); % Open file for read access

    % Skip through to the start of the output
    buffer = fgetl(fid);
    while isempty(strfind(buffer,'======================='))
        if ~isempty(strfind(buffer,'Time  |'))
            error('Did you try and open and old-style output file? Found a | that looked like a delimiter')
        end
        buffer = fgetl(fid);
    end
    fgetl(fid);

    headers = fgetl(fid);
    nodes = fgetl(fid);

    
    nf.fields = strsplit(headers);
    nf.fields = nf.fields(2:end-1);

    [nf.nodes,base_index] = get_nodes(nodes,nf.fields);


    % if himem
    %     data = zeros(nrows,sum(cellfun(@length,nf.nodes)));
    %     for j = 1:nrows
    %         tmp = textscan(fid,'%f',1,'CollectOutput',true);
    %         data(j,:) = tmp{1};
    %     end
    % else
    data = textscan(fid,'%f','CollectOutput',true);
    data = reshape(data{1},length(nf.fields),[]).';
    %end

    for j = 1:length(nf.nodes)
        nf.data{j} = data(:,base_index{j});
    end

    % idx_start = 1;
    % for j = 1:length(nf.nodes) % For each output trace
    %     nf.data{j} = data(:,idx_start:idx_start+length(nf.nodes{j})-1);
    %     idx_start = idx_start + length(nf.nodes{j});
    % end    
    fclose(fid);
    
    % Finally, move the time to an array of its own
    nf.time = nf.data{1};
    nf.data = nf.data(2:end);
    nf.fields = unique(nf.fields(2:end),'stable');
    nf.nodes = nf.nodes(2:end);
    
    nf.deltat = nf.time(2)-nf.time(1);
    nf.npoints = length(nf.time);
end



function [locs,base_index] = get_nodes(line,headers)
    % Given a line with contents like '1 | 1 2 | 1 |
    % return a number of nodes {1,1:2,1}
    % nentries stores the number of entries per line
    nodes = cellfun(@(x) str2double(x),strsplit(line(1:end-2)));
    nodes(1) = 1; % Number of time columns (there is only ever one)
    traces = unique(headers,'stable');
    locs = cell(1,length(traces));
    base_index = locs;
    for j = 1:length(traces)
        base_index{j} = strcmp(traces{j},headers);
        locs{j} = nodes(base_index{j});
    end
end

% function numlines = count_lines(fname)
%     if (isunix) %# Linux, mac
%         [status, result] = system( ['wc -l ',fname,' | cut -d'' '' -f1'] );
%         numlines = str2num(result);

%     elseif (ispc) %# Windows
%         numlines = str2num( perl('countlines.pl',fname) );

%     else
%         error('...');

%     end
%end