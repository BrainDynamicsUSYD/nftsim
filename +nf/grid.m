%% This function reshapes a 2D array into a 3D array, with the 1st and 2nd dimensions, 
%  corresponding to X and Y coordinates and the 3rd dimension corresponding to time. 
%
% e.g. data = nf.grid(nf,'Propagator.2.phi') will return the data for Propagator.2.phi
% as a 3D matrix, with X and Y coordinates referring to grid positions and
% Z corresponding to time. This script will throw an error if the output nodes
% do not start at 1; are not consecutive (i.e. the conf file should
% output all nodes), or the number of nodes is not a perfect square..
%
% ARGUMENTS:
%        obj   -- A nftsim output struct (a Matlab struct containing data
%                 from a simulation).
%        trace -- A string with the name of the array to reshape. 
%
% OUTPUT:
%        data  -- The reshaped array specified in trace.
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

    
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [data, longside_nodes, shortside_nodes] = grid(obj, trace)
    %
    output_nodes = length(obj.nodes{1});
    
    if output_nodes ~= obj.input_nodes 
        error(['nf:' mfilename ':IncompatibleOutput'], ...
              'Output from NFTsim must be for all nodes')
    end    

    data = nf.extract(obj, trace);
    %TODO: handle non-square grid here.
    
    if ~isfield(obj, 'longside_nodes')
        longside_nodes  = sqrt(output_nodes); % length of square grid edge
        shortside_nodes = longside_nodes;
    else
        longside_nodes  = obj.longside_nodes;
        shortside_nodes = obj.input_nodes / obj.longside_nodes;
    end
    %Reshape to an array of (nx,ny,tpts)

    %data = reshape(data, grid_edge, grid_edge, obj.npoints);
    
    data = shiftdim(reshape(data, [], longside_nodes, shortside_nodes), 1);
end %function grid()
