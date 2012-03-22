function [fields,nodes] = nfinfo( filename )
%NFINFO returns information about neurofield.output

fid = fopen(filename);
temp = fgetl(fid);
[~,~,~,temp,~,~,~] = regexpi( temp, '[A-Z0-9\.]+' );

fields = unique(temp);
for ii = 1:length(fields)
    if strcmp( fields{ii}, 'Time' )
        fields{ii} = [];
    end
end

temp = fgetl(fid);
[~,~,~,temp,~,~,~] = regexpi( temp, '[0-9]+' );
temp = unique(temp);
nodes = {};
for ii = 1:length(temp)
    nodes = [nodes;temp{ii}];
end

fclose(fid);

end

