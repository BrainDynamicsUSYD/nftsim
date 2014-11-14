function nf = nf_read(fname,himem);
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
    
    if himem
        nrows = count_lines(fname) - 2;
    end

    fid = fopen(fname,'r'); % Open file for read access
    headers = fgetl(fid);
    nodes = fgetl(fid);

    nf.fields = get_fields(headers);
    nf.nodes = get_nodes(nodes);

    format_string = '%f'; % Read the time
    for j = 2:length(nf.nodes) % And any others
        format_string = sprintf('%s | %s',format_string,repmat('%f ',1,length(nf.nodes{j}))); % repmat tiles array of '%f'
    end 
    format_string = sprintf('%s |',format_string);

    if himem
        data = zeros(nrows,sum(cellfun(@length,nf.nodes)));
        for j = 1:nrows
            tmp = textscan(fid,format_string,1,'CollectOutput',true);
            data(j,:) = tmp{1};
        end
    else
        data = textscan(fid,format_string,'CollectOutput',true);
        data = data{1};
    end

    idx_start = 1;
    for j = 1:length(nf.nodes) % For each output trace
        nf.data{j} = data(:,idx_start:idx_start+length(nf.nodes{j})-1);
        idx_start = idx_start + length(nf.nodes{j});
    end    
    fclose(fid);
    
    % Finally, move the time to an array of its own
    nf.time = nf.data{1};
    nf.data = nf.data(2:end);
    nf.fields = nf.fields(2:end);
    nf.nodes = nf.nodes(2:end);
    
    nf.deltat = nf.time(2)-nf.time(1);
    nf.npoints = length(nf.time);
end


function headers = get_fields(line)
    % Given a line with contents 'Time | Propag2.V Propag2.V | Couple1.nu |
    % Return a cell {'Time','Propag2.V','Couple1.nu'}
    a = regexp(line,'[| ]*','split');
    
    % This loop ensures that the original order is preserved, unlike unique()
    headers = {};
    for j = 1:length(a)
        if ~isempty(a{j}) && ~any(strcmp(headers,a{j})) 
            headers{end+1} = a{j};
        end
    end
end

function [nodes,nentries] = get_nodes(line)
    % Given a line with contents like '| 1 2 | 1 |
    % return a number of notes {1,1:2,1}
    % nentries stores the number of entries per line
    nodes{1} = 1;
    delims = strfind(line,'|');
    for j = 1:length(delims)-1
        nodes{j+1} = str2num(line(delims(j)+1:delims(j+1)-1));
    end
end

function numlines = count_lines(fname)
    if (isunix) %# Linux, mac
        [status, result] = system( ['wc -l ',fname,' | cut -d'' '' -f1'] );
        numlines = str2num(result);

    elseif (ispc) %# Windows
        numlines = str2num( perl('countlines.pl',fname) );

    else
        error('...');

    end
end