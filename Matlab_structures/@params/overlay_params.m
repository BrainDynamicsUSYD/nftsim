function t = overlay_params(pars,loc,fig)
	% Print the model parameters on the current axes
	if nargin < 3 || isempty(fig)
		fig = gcf;
	end

	if nargin < 2 || isempty(loc)
		loc = 'ne';
	end

	offset = 0.02; 

	switch loc
		case 'ne'
			pos = [1-offset 1-offset];
			valign = 'top';
			halign = 'right';
		case 'nw'
			pos = [offset 1-offset];
			valign = 'top';
			halign = 'left';
		case 'se'
			pos = [1-offset offset];
			valign = 'bottom';
			halign = 'right';
		case 'sw'
			pos = [offset offset];
			valign = 'bottom';
			halign = 'left';
	end

	textdata = {};
	vars = fields(pars);

	if isempty(vars)
		return
	end

	for j = 1:length(vars)
		if strfind(vars{j},'spatial')
			continue
		end
		if strcmp(vars{j},'disable_set')
			continue
		end

		val = pars.(vars{j});

		if strcmp(vars{j},'alpha') || strcmp(vars{j},'beta') && all(val == val(1))
			val = val(1);
		end

		if isstr(val)
			reptok = '%s';
		elseif isnumeric(val)
			reptok = '';
			septok = '';
			
			for k = 1:length(val)
				if k > 1
					septok = ',';
				end
				int_like = abs(round(val(k))-val(k)) < 1e-7; % The number looks like an integer unless it is super-small
				if abs(val(k)) < 1e-3 || ( abs(val(k)) > 1e3 && ~int_like) || abs(val(k)) > 1e6 
					% For very large numbers, use exponential notation. 
					% The 1e-2 check above matches the number of DPs for the %.xf float format below
					reptok = sprintf('%s%s%s',reptok,septok,'%.2e');
				elseif int_like
					reptok = sprintf('%s%s%s',reptok,septok,'%.0f');
				else
					reptok = sprintf('%s%s%s',reptok,septok,'%.3f');
				end
			end
			if length(val) > 1
				reptok = sprintf('[%s]',reptok);
			end
		end

		outstr = sprintf('%s = %s',vars{j},reptok);
		textdata{j} = sprintf(outstr,val);
	end

	t = text(1,1,textdata,'Units','normalized','VerticalAlignment',valign,'HorizontalAlignment',halign,'FontSize',7);
	t.Position = pos;
