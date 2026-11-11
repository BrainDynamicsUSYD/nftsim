%% Tests for the +nf analysis functions fixed in the 2026 wrapper audit.
%
% Build nftsim first (make nftsim), then from the repository root run
%
%     matlab -batch "run('test/matlab/test_analysis.m')"
%
% The script errors on the first failure.

root = fileparts(fileparts(fileparts(mfilename('fullpath'))));
addpath(root);
nftsim = fullfile(root, 'bin', 'nftsim');
assert(exist(nftsim, 'file') == 2, 'Build nftsim first: make nftsim');
tmp = tempname;
mkdir(tmp);
cleanup = onCleanup(@() rmdir(tmp, 's'));
copyfile(fullfile(root, 'test', 'numerical', 'eirs-dendrite-integral.conf'), fullfile(tmp, 'eirs.conf'));
evalc('obj = nf.run(fullfile(tmp, ''eirs''), false, nftsim);');

% nf.report used a field (conf_file) that nf.read never sets.
evalc('nf.report(obj)');
fprintf('OK   nf.report runs\n');

% nf.read records the sheet length.
assert(isfield(obj, 'length') && obj.length == 0.5, 'nf.read did not record Length');
fprintf('OK   nf.read records Length\n');

% nf.spectrum: [] for an optional argument means its default (one-sided).
[f1, P1] = nf.spectrum(obj, 'Propagator.1.phi');
[f2, P2] = nf.spectrum(obj, 'Propagator.1.phi', [], [], [], []);
assert(isequal(f1, f2) && isequal(P1, P2) && min(f2) == 0, '[] arguments did not give the defaults');
fprintf('OK   nf.spectrum treats [] as the default\n');

% nf.rfft: the PSD of unit-variance white noise integrates to 1, windowed or not.
rng(1);
y = randn(2^16, 1);
fs = 1000;
[f, ~, Pu] = nf.rfft(y, fs, [], false);
[~, ~, Pw] = nf.rfft(y, fs, [], true);
df = f(2) - f(1);
assert(abs(sum(Pu)*df - 1) < 0.02 && abs(sum(Pw)*df - 1) < 0.02, ...
       'PSD integrals %.3f (unwindowed), %.3f (windowed)', sum(Pu)*df, sum(Pw)*df);
assert(numel(f) == numel(Pu), 'frequency axis length differs from the PSD');
[f3, ~, P3] = nf.rfft(y(1:1001), fs, [], false, false, false); % odd length, two-sided
assert(numel(f3) == numel(P3) && any(f3 == 0), 'two-sided axis for odd lengths');
fprintf('OK   nf.rfft PSD normalisation and frequency axes\n');

% nf.wavelet_spectrogram, from a struct and from a time series (needs the Wavelet Toolbox).
if exist('cwtft', 'file')
    [cfx, cf] = nf.wavelet_spectrogram(obj, 'Propagator.1.phi', linspace(2, 40, 20));
    assert(size(cfx, 1) == 20 && numel(cf) == 20, 'wavelet from a struct');
    t = 0:1e-3:2;
    [cfx2, ~] = nf.wavelet_spectrogram(sin(2*pi*10*t), 1000, linspace(2, 40, 20));
    assert(isequal(size(cfx2), [20, numel(t)]), 'wavelet from a time series');
    fprintf('OK   nf.wavelet_spectrogram from a struct and a time series\n');
else
    fprintf('SKIP nf.wavelet_spectrogram (no Wavelet Toolbox)\n');
end
fprintf('All analysis tests passed.\n');
