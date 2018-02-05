function test_output(save_new)
	% This function tests the latest nftsim against known results
	% nf_run, nf_read and nf_spatial_spectrum need to be on the path
	if nargin < 1 || isempty(save_new)
		save_new = 0;
	end

	fnames = {'eirs_eyes_closed','eirs_eyes_closed_2','eirs_spindles'};
	for j = 1:length(fnames)
		nf = nf_run(fnames{j});
		[f1,P1] = nf_spatial_spectrum(nf);
		if ~exist([fnames{j},'.mat'])
			f = f1;
			P = P1;
			save(fnames{j});
			continue
		end
		s = load(fnames{j});
		figure
		%loglog(s.fa,s.Pa,'g');
		loglog(s.f,s.P,'b','LineWidth',1);
		hold on
		loglog(f1,P1,'r--');
		%legend('Analytic','Expected result','Current result','Location','SouthWest');
		title(sprintf('Test case: %s',fnames{j}),'Interpreter','none');
		drawnow
	end
