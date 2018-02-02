%% Function to run nftsim and return a nftsim output struct.
%
% Provided a configuration file-name (fname.conf), run the nftsim
% executable, generating an output file (fname.output). Optionally, if an
% output argument is provided then, parse the output file and return a
% nftsim output struct containing the simulation results.
%
%
% ARGUMENTS:
%        fname -- Name of the configuration file, it can be with or without
%                 the .conf extension.
%        time_stamp -- boolean flag to use a time_stamp YYYY-MM-DDTHHMMSS
%                      in the output file name.
%        nftsim_path -- nftsim executable (full or relative path).
%
% OUTPUT: Writes a .output file in the same location as the .conf file.
%        obj -- A nftsim output struct (a Matlab struct containing
%               data from a simulation).
%
% REQUIRES:
%        nftsim -- The nftsim executable, must be in your path.
%        nf.read -- Read a nftsim output file and return a nftsim
%                   output struct.
%
% AUTHOR:
%     Romesh Abeysuriya (2012-03-22).
%
% USAGE:
%{
    %At a matlab command promt, from nftsim's main directory:
    nf_obj = nf.run('./configs/eirs-corticothalamic.conf')
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function obj = run(fname, time_stamp, nftsim_path)
    %
    tic;
    fname = strrep(fname, '.conf', ''); %Strip any .conf suffix.
    fprintf(1, 'INFO: Using configuration file: %s.conf...\n', fname);

    if nargin < 2
        time_stamp = false;
    end
    % If we were not provided a path to nftsim, try to determine one.
    if nargin < 3 || isempty(nftsim_path)
        % Check typical locations, the first path that exists will be selected.
        locations = {'nftsim', ...
                     './bin/nftsim', ...
                     './nftsim/bin/nftsim', ...
                     'nftsim.exe'};
        selected_path = find(cellfun(@(name) exist(name, 'file')==2, locations), 1, 'first');
        if ~isempty(selected_path)
            nftsim_path = locations{selected_path};
        else
            error(['nf:' mfilename ':BadPath'], ...
                  'nftsim not found. Either change into the nftsim base directory or make a symlink to nftsim in the current directory.');
        end
    % If we were provided a path, check that it is valid.
    elseif ~exist(nftsim_path, 'file')
        error(['nf:' mfilename ':BadPath'], ...
              'The nftsim_path you provided is incorrect:"%s".',nftsim_path);
    end

    if ~time_stamp
        nftsim_cmd = sprintf('%s -i %s.conf -o %s.output', nftsim_path, fname, fname);
    else
        nftsim_cmd = sprintf('%s -i %s.conf -t', nftsim_path, fname);
    end
    fprintf('INFO: Executing command:\n')    
    fprintf('%s\n', nftsim_cmd);
    [status, cmdout] = system(nftsim_cmd);

    string(cmdout)
    if status ~= 0
        error(['nf:' mfilename ':NFTsimError'], ...
              'An error occurred while running nftsim!');
    end

    fprintf(1, 'INFO: tic-toc: Took about %.3f seconds\n', toc);

    if nargout > 0
        fprintf(1, 'Parsing output...');
        if time_stamp
            cmdout = string(cmdout);
            temp_struct = regexp(cmdout,'(?<stamp>\d+-\d+-\d+T\d+)', 'names');
            fname = strcat(fname, '_', temp_struct.stamp);
        end
            obj = nf.read(sprintf('%s.output', fname));
            fprintf(1, 'done!\n');
    end
end %function run()
