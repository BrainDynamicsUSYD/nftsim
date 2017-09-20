%% Function to run neurofield and return a neurofield output struct.
%
% Provided a configuration file-name (fname.conf), run the neurofield
% executable, generating an output file (fname.output). Optionally, if an
% output argument is provided then, parse the output file and return a
% neurofield output struct containing the simulation results.
%
%
% ARGUMENTS:
%        fname -- Name of the configuration file, it can be with or without
%                 the .conf extension.
%        timestamp -- boolean flag to use a timestamp YYYY-MM-DDTHHMMSS in 
%                     the output file name. 
%        neurofield_path -- neurofield executable (full or relative path).
%
% OUTPUT: Writes a .output file in the same location as the .conf file.
%        obj -- A neurofield output struct (a Matlab struct containing
%               data from a simulation).
%
% REQUIRES:
%        neurofield -- The neurofield executable, must be in your path.
%        nf.read -- Read a neurofield output file and return a neurofield
%                   output struct.
%
% AUTHOR:
%     Romesh Abeysuriya (2012-03-22).
%
% USAGE:
%{
    %
    
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function obj = run(fname, timestamp, neurofield_path)
    %
    tic;
    fname = strrep(fname, '.conf', ''); %Strip any .conf suffix.
    fprintf(1, 'INFO: Using configuration file: %s.conf...\n', fname);

    if nargin < 2
        timestamp = false;
    end
    % If we were not provided a path to neurofield.
    if nargin < 3 || isempty(neurofield_path)
        % Check typical locations, the first path that exists will be selected.
        locations = {'neurofield', ...
                     './bin/neurofield', ...
                     './neurofield/bin/neurofield', ...
                     'neurofield.exe'};
        selected_path = find(cellfun(@(name) exist(name, 'file')==2, locations), 1, 'first');
        if ~isempty(selected_path)
            neurofield_path = locations{selected_path};
        end
        
    end

    % Check we have a valid path to the neurofield executable.
    if ~exist(neurofield_path, 'file')
        error(['nf:' mfilename ':BadPath'], ...
               'neurofield not found. Please make a symlink to neurofield in the current directory.');
    end

    if ~timestamp
        neurofield_cmd = sprintf('%s -i %s.conf -o %s.output', neurofield_path, fname, fname);
    
    else
        neurofield_cmd = sprintf('%s -i %s.conf -t', neurofield_path, fname);
    end
    fprintf('INFO: Executing command:\n')    
    fprintf('%s\n', neurofield_cmd);
    [status, cmdout] = system(neurofield_cmd);

    string(cmdout)
    if status ~= 0
        error(['nf:' mfilename ':NeurofieldError'], ...
              'An error occurred while running neurofield!');
    end

    fprintf(1, 'INFO: tic-toc: Took about %.3f seconds\n', toc);

    if nargout > 0
        fprintf(1, 'Parsing output...');
        if timestamp
            cmdout = string(cmdout);
            temp_struct = regexp(cmdout,'(?<stamp>\d+-\d+-\d+T\d+)', 'names');
            fname = strcat(fname, '_', temp_struct.stamp);            
        end
            obj = nf.read(sprintf('%s.output', fname));
            fprintf(1, 'done!\n');
    end
end %function run()
