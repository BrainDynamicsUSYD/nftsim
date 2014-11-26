function [bands,bandstr] = eeg_bands()
	% This function returns the boundaries for each EEG band
	bands = [1 4;... % delta
	 4 7.5; ... % theta
	 7.5 13; ... % alpha
	 11 16; ... % sigma (overlaps alpha and beta)
	 13 30; ...  % beta
	 30 45; ... % gamma
	 ];
	 
	 if nargout > 1
	 	bandstr = {'delta','theta','alpha','sigma','beta','gamma'};
	 end
	 
	 % bands = [0.5 4;... % delta
	 %  4 8; ... % theta
	 %  8 12; ... % alpha
	 %  11 16; ... % sigma (overlaps alpha and beta)
	 %  16 30; ...  % beta
	 %  30 45; ... % gamma
	 %  ];

	 % WON: 0.5-4.5, 4.5-8, 8-12, 12-15, 15-32 for each band (zero overlap or gaps)
