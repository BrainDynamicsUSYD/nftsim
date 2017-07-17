%% Function to run NeuroField and return a NF struct.
%
% Provide the filename WITHOUT .conf
% Program will
% - read fname.conf
% - output fname.dump and fname.output
% - parse fname.output and return an NF struct
%
% If the user specifies fname = 'fname.conf' the '.conf' will be removed
% Requires 'neurofield' to be on the path
%
% ARGUMENTS:
%        fname -- .
%        neurofield_path -- .
%
% OUTPUT:
%        obj -- .
%
% REQUIRES:
%        neurofield -- must be in your path.
%        nf.read -- .
%
% AUTHOR:
%     Romesh Abeysuriya (2012-03-22).
%
% USAGE:
%{
    %
    
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function obj = run(fname, neurofield_path)
    %
    tic;
    fname = strrep(fname, '.conf', '');
    fprintf(1, 'Executing NeuroField: %s.conf...\n', fname);

    if nargin < 2 || isempty(neurofield_path)
        % The first path that exists in will be selected
        locations = {'neurofield.exe', './neurofield/bin/neurofield', './bin/neurofield', 'neurofield'};
        selected_path = find(cellfun(@(name) exist(name, 'file')==2, locations), 1, 'first');
        if isempty(selected_path)
            error('neurofield not found. Please make a symlink to neurofield in the current directory');
        else
            neurofield_path = locations{selected_path};
        end
    end

    neurofield_cmd = sprintf('%s -i %s.conf -o %s.output', neurofield_path, fname, fname);
    fprintf('%s\n', neurofield_cmd);
    [status] = system(neurofield_cmd);

    if status ~= 0
        error('An error occurred when running NeuroField!');
    end

    fprintf(1, 'took %.3f seconds\n', toc);
    
    if nargout > 0
        fprintf(1, 'Parsing output...');
        obj = nf.read(sprintf('%s.output', fname));
        fprintf(1, 'done!\n');
    end
end %function run()
