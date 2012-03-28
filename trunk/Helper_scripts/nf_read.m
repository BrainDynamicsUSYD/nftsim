function nf = nf_read(fname);
    % nf_read = read_nf(fname);
    % Read a neurofield output file and return a neurofield output struct
    % for use with other helper scripts
    % Romesh Abeysuriya 120322
    fid = fopen(fname,'r'); % Open file for read access
    headers = fgetl(fid);
    nodes = fgetl(fid);
    
    nf.fields = get_fields(headers);
    nf.nodes = get_nodes(nodes);

    format_string = '%f'; % Read the time
    for j = 2:length(nf.nodes) % And any others
        format_string = sprintf('%s | %s',format_string,repmat('%f ',1,length(nf.nodes{j})));
    end
    format_string = sprintf('%s |',format_string);
    data = textscan(fid,format_string,'CollectOutput',1);
    data = data{1};
    
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
