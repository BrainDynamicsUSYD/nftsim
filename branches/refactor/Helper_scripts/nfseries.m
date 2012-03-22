function [t,x] = nfseries( filename, field, node )
%NFSERIES returns requested time series

    % read the number of nodes
    [~,nodes] = nfinfo(filename); nodes = length(nodes);

    % get the number of lines in file
    lines = numel(textread( filename, '%1c%*[^\n]' ));
    lines = lines-2; % don't count preamble lines

    fid = fopen(filename);
    fgetl(fid); fgetl(fid); % throw away preamble

    t = zeros(1,lines);
    for ii = 1:lines
        [temp] = textscan( fid, '%f | ' ) % read time
        t(ii) = temp{1}(1);
        asdf = textscan( fid, [repmat('%f',1,nodes),'  |'] )
        textscan(fid,'\n');
    end
    fclose(fid);
end
