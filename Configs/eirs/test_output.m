function test_output()
	% This function tests the latest neurofield against known results
	% nf_run, nf_read and nf_spatial_spectrum need to be on the path

	fnames = {'eirs_eyes_closed','eirs_eyes_closed_2','eirs_spindles'};
	for j = 1:length(fnames)
		nf = nf_run(fnames{j});
		[f1,P1] = nf_spatial_spectrum(nf);
		s = load(fnames{j});
		figure
		loglog(s.fa,s.Pa,'g');
		hold on
		loglog(s.f,s.P,'b','LineWidth',2);
		loglog(f1,P1,'r--');
		legend('Analytic','Expected result','Current result','Location','SouthWest');
		title(sprintf('Test case: %s',fnames{j}),'Interpreter','none');
		drawnow
	end