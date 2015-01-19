qfunction [data,grid_edge] = nf_grid(nf,trace)
    % This function returns a gridded version of a single parameter
    % e.g. data = nf_grid(nf,'Propag.2.phi') will return the data for Propag.2.phi
    % as a 3D matrix, with X and Y coordinates referring to grid positions and
    % Z corresponding to time. This script will throw an error if the output nodes
    % do not start at 1 or are not consecutive (i.e. the conf file should output all nodes)

    nodes = nf.nodes{1};
    if nodes(1) ~= 1 % || any(diff(nodes))~=1
        error('Output from NeuroField must be for all nodes')
    end
    
    data = nf_extract(nf,trace);
    
    grid_edge = sqrt(length(nodes)); % length of square grid edge
    
    %data = reshape(data,grid_edge,grid_edge,nf.npoints);
    
    data = shiftdim(reshape(data,[],grid_edge,grid_edge),1);
