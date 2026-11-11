%% Tests for nf.run's output format: binary by default, Text on request, and
% the config's own Format: respected when no format is given.
%
% Build nftsim first (make nftsim), then from the repository root run
%
%     matlab -batch "run('test/matlab/test_run.m')"
%
% The script errors on the first failure.

root = fileparts(fileparts(fileparts(mfilename('fullpath'))));
addpath(root);
nftsim = fullfile(root, 'bin', 'nftsim');
assert(exist(nftsim, 'file') == 2, 'Build nftsim first: make nftsim');
tmp = tempname;
mkdir(tmp);
cleanup = onCleanup(@() rmdir(tmp, 's'));

src = fileread(fullfile(root, 'test', 'numerical', 'e-erps.conf'));
is_binary = @(path) contains(fileread(path), 'NFTSIM-BINARY');
% Writes a copy of e-erps.conf, optionally with a Format: line in its Output section.
write_conf = @(name, format_line) write_file(fullfile(tmp, [name '.conf']), ...
    regexprep(src, '(\n\s*Output:[^\n]*)', ['$1' format_line], 'once'));

% 1. No format given and none in the config: binary.
write_conf('default', '');
evalc('obj = nf.run(fullfile(tmp, ''default''), false, nftsim);');
assert(is_binary(fullfile(tmp, 'default.output')), 'default: expected binary output');
fprintf('OK   default output is binary\n');

% 2. Explicit Text: text, and the same values as the binary run.
write_conf('text', '');
evalc('obj_text = nf.run(fullfile(tmp, ''text''), false, nftsim, ''Text'');');
assert(~is_binary(fullfile(tmp, 'text.output')), 'Text: expected text output');
assert(isequal(obj.fields, obj_text.fields) && obj.npoints == obj_text.npoints, 'Text vs binary: structure differs');
for j = 1:length(obj.fields)
    scale = max(abs(obj_text.data{j}(:)));
    assert(max(abs(obj.data{j}(:) - obj_text.data{j}(:))) <= 1e-13*max(scale, realmin), ...
           'Text vs binary: %s differs', obj.fields{j});
end
fprintf('OK   explicit Text gives text output with the same values\n');

% 3. Config says Format: Text and no format is given: the config is respected.
write_conf('cfgtext', ' Format: Text');
evalc('nf.run(fullfile(tmp, ''cfgtext''), false, nftsim);');
assert(~is_binary(fullfile(tmp, 'cfgtext.output')), 'config Format: Text was overridden');
fprintf('OK   config Format: Text is respected\n');

% 4. Config says Format: Binary32: float32 binary.
write_conf('cfg32', ' Format: Binary32');
evalc('nf.run(fullfile(tmp, ''cfg32''), false, nftsim);');
assert(contains(fileread(fullfile(tmp, 'cfg32.output')), 'dtype=float32'), 'config Format: Binary32 not used');
fprintf('OK   config Format: Binary32 is respected\n');

% 5. An explicit format overrides the config.
write_conf('override', ' Format: Text');
evalc('nf.run(fullfile(tmp, ''override''), false, nftsim, ''Binary'');');
assert(is_binary(fullfile(tmp, 'override.output')), 'explicit Binary did not override the config');
fprintf('OK   an explicit format overrides the config\n');

% 6. An unknown format is an error.
try
    evalc('nf.run(fullfile(tmp, ''default''), false, nftsim, ''csv'');');
    error('test:noError', 'an unknown format was accepted');
catch err
    assert(strcmp(err.identifier, 'nf:run:BadFormat'), 'unexpected error: %s', err.message);
end
fprintf('OK   an unknown format is rejected\n');
fprintf('All nf.run tests passed.\n');

function write_file(path, text)
    fid = fopen(path, 'w');
    fwrite(fid, text);
    fclose(fid);
end
