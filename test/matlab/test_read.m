%% Tests for the Matlab output reader, +nf/read.m, on text and binary output.
%
% Runs bin/nftsim on a few test/numerical configurations in the text, Binary
% and Binary32 formats and checks that nf.read returns the same struct for
% each, with binary values equal to the text ones to within text's 15
% significant digits (binary32 to float32 precision). Also checks that a file
% cut off mid-row (an interrupted run) is read up to its last complete row,
% with a warning. Build nftsim first (make nftsim), then from the repository
% root run
%
%     matlab -batch "run('test/matlab/test_read.m')"
%
% The script errors on the first failure.

root = fileparts(fileparts(fileparts(mfilename('fullpath'))));
addpath(root);
nftsim = fullfile(root, 'bin', 'nftsim');
assert(exist(nftsim, 'file') == 2, 'Build nftsim first: make nftsim');
tmp = tempname;
mkdir(tmp);
cleanup = onCleanup(@() rmdir(tmp, 's'));

configs = {'e-erps', 'wave-rectangular-grid', 'eirs-dendrite-integral'};
formats = {'Text', 'Binary', 'Binary32'};
for c = 1:length(configs)
    conf = fullfile(root, 'test', 'numerical', [configs{c} '.conf']);
    out = struct();
    for f = 1:length(formats)
        path = fullfile(tmp, [configs{c} '.' formats{f} '.output']);
        status = system(sprintf('"%s" -i "%s" -o "%s" --format %s', nftsim, conf, path, formats{f}));
        assert(status == 0, 'nftsim failed on %s', conf);
        out.(formats{f}) = nf.read(path);
    end
    text = out.Text;
    for f = 2:length(formats)
        bin = out.(formats{f});
        assert(isequal(fieldnames(bin), fieldnames(text)), 'struct fields differ');
        assert(isequal(bin.fields, text.fields) && isequal(bin.nodes, text.nodes) ...
               && bin.npoints == text.npoints, '%s %s: fields, nodes or npoints differ', configs{c}, formats{f});
        if strcmp(formats{f}, 'Binary')
            rtol = 1e-14; % text has 15 significant digits
        else
            rtol = 1e-7;  % float32
        end
        close_enough = @(a, b) all(abs(a(:) - b(:)) <= rtol * max(abs(b(:))) + realmin);
        assert(close_enough(bin.time, text.time) && abs(bin.deltat - text.deltat) <= rtol * text.deltat, ...
               '%s %s: time differs', configs{c}, formats{f});
        for j = 1:length(text.data)
            assert(close_enough(bin.data{j}, text.data{j}), '%s %s: %s differs', ...
                   configs{c}, formats{f}, text.fields{j});
        end
        fprintf('OK   %s %s: %d rows, %d traces\n', configs{c}, formats{f}, bin.npoints, length(bin.fields));
    end
end

% Interrupted runs: cut a file in the middle of its last row.
for f = 1:length(formats)
    full_path = fullfile(tmp, ['e-erps.' formats{f} '.output']);
    whole = nf.read(full_path);
    fid = fopen(full_path, 'r');
    bytes = fread(fid, Inf, 'uint8=>uint8');
    fclose(fid);
    cut_path = fullfile(tmp, ['cut.' formats{f} '.output']);
    fid = fopen(cut_path, 'w');
    fwrite(fid, bytes(1:end-5)); % less than a row in every format
    fclose(fid);
    lastwarn('');
    warning('off', 'nf:read:IncompleteOutput');
    cut = nf.read(cut_path);
    warning('on', 'nf:read:IncompleteOutput');
    [~, id] = lastwarn();
    assert(strcmp(id, 'nf:read:IncompleteOutput'), '%s: no warning for a cut file', formats{f});
    assert(cut.npoints == whole.npoints - 1, '%s: expected %d rows, read %d', ...
           formats{f}, whole.npoints - 1, cut.npoints);
    assert(isequal(cut.time, whole.time(1:end-1)) && isequal(cut.data{1}, whole.data{1}(1:end-1, :)), ...
           '%s: values of a cut file differ', formats{f});
    fprintf('OK   cut %s file: %d of %d rows\n', formats{f}, cut.npoints, whole.npoints);
end
fprintf('All nf.read tests passed.\n');
