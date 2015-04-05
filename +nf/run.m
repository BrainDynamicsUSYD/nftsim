function obj = run(fname,neurofield_path)
    % Function to run NeuroField and return a NF struct
    % Provide the filename WITHOUT .conf
    % Program will
    % - read fname.conf
    % - output fname.dump and fname.output
    % - parse fname.output and return an NF struct
    % 
    % If the user specifies fname = 'fname.conf' the '.conf' will be removed
    % Requires 'neurofield' to be on the path
    
    tic;
    fname = strrep(fname,'.conf','');
    fprintf(1,'Executing NeuroField: %s.conf...\n',fname);

    if nargin < 2 || isempty(neurofield_path)
        if ispc
            neurofield_path = 'neurofield.exe';
        else
            neurofield_path = './neurofield/bin/neurofield';
            if ~exist(neurofield_path) 
                neurofield_path = 'neurofield'; % Legacy functionality
            end
        end
    end
    
    if ~exist(neurofield_path) 
        error('neurofield could not be found. Please make a symlink to neurofield in the current directory');
    end

    neurofield_cmd = sprintf('%s -i %s.conf -o %s.output',neurofield_path,fname,fname);
    fprintf('%s\n',neurofield_cmd);
    [status] = system(neurofield_cmd);

    if status ~= 0
        error('An error occurred when running NeuroField!');
    end

    fprintf(1,'took %.3f seconds\n',toc);
    
    if nargout > 0
        fprintf(1,'Parsing output...');
        obj = nf.read(sprintf('%s.output',fname));   
        fprintf(1,'done!\n');
    end
