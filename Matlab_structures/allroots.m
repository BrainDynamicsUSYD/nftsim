function [rootvals,discard] = allroots(fn,x,varargin)
% Find all roots of a function fn along values x
% 
% allroots(fn,x)
%
% This function can accept either a function handle or
% an array of values
% eg. r = allroots(@ve_root,xvals)
%     r = allroots(yvals,xvals)

% Evaluate the function along x
discard = 0;
rootvals = [];
if strcmp(class(fn),'function_handle')
	y = feval(fn,x,varargin{:});
else
	y = fn;
end

% Find roots (coarse)
r_ind = find(diff(sign(y))); % Diff to find where signs change
if isempty(r_ind); return; end; % If there are no roots, return immediately
r_ind = r_ind(isfinite(y(r_ind))); 
r_ind(diff(r_ind)==1) = []; % Use exact roots where possible

% Check the gradient nearby- find turning points
% Method is to take derivative, then search for sign changes (turning points)
tps = 1 + find(diff(sign(diff(y))));

% Remove roots which are also turning points
for j = 1:length(r_ind)
	if any(tps==r_ind(j))
		r_ind(j) = 0;
		discard = 1;
	end
end		

r_ind = r_ind(find(r_ind));
rootvals = x(r_ind);

% Exit if y is an array rather than a function handle
if strcmp(class(fn),'double')
	return
end

% Otherwise refine roots
opt.Display = 'none';
for j = 1:length(r_ind)
	% Explicity specify the range over which the root occurs
	% WARNING- This may result in array-out-of-bounds
	% ...but I think that because of the way r_ind is constructed,
	% j can never be the last entry in x
	try
		rootvals(j) = fzero_fast(fn,[x(r_ind(j)) x(r_ind(j)+1)],opt,varargin{:});
	catch
		[x(r_ind(j)) x(r_ind(j)+1)]
		feval(fn,[x(r_ind(j)) x(r_ind(j)+1)])
		error('Error in allroots.m')
	end
end

rootvals = rootvals(isfinite(rootvals));
