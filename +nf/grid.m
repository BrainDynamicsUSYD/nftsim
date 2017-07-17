%% This function returns a gridded version of a single parameter.
%
% e.g. data = nf.grid(nf,'Propagator.2.phi') will return the data for Propagator.2.phi
% as a 3D matrix, with X and Y coordinates referring to grid positions and
% Z corresponding to time. This script will throw an error if the output nodes
% do not start at 1; are not consecutive (i.e. the conf file should
% output all nodes), or the number of nodes is not a perfect square..
%
% ARGUMENTS:
%        obj -- nf object
%        trace -- .
%
% OUTPUT:
%        data -- .
%        grid_edge -- .
%
% REQUIRES:
%        nf.extract -- <description>
%
% AUTHOR:
%     Romesh Abeysuriya (2012-03-22).
%
% USAGE:
%{
    %
    
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [data, grid_edge] = grid(obj, trace)
    %
    nodes = obj.nodes{1};
    if nodes(1) ~= 1 % || any(diff(nodes))~=1
        error('Output from NeuroField must be for all nodes')
    end
    
    data = nf.extract(obj, trace);
    
    grid_edge = sqrt(length(nodes)); % length of square grid edge
    
    %data = reshape(data, grid_edge, grid_edge, obj.npoints);
    
    data = shiftdim(reshape(data, [], grid_edge, grid_edge), 1);
end %function grid()
