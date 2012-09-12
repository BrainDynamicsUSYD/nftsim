function nf = nf_run(fname)
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
    fprintf(1,'Executing NeuroField: %s.conf...',fname);
    [status] = system(sprintf('neurofield -i %s.conf -d %s.dump -o %s.output',fname,fname,fname));
    if status ~= 0
        error('An error occurred when running Neurofield. Make sure ''neurofield'' is on your path');
    end
    fprintf(1,'took %.3f seconds\n',toc);
    
    if nargout > 0
        fprintf(1,'Parsing output...');
        nf = nf_read(sprintf('%s.output',fname));   
        fprintf(1,'done!\n');
    end
