function [b,fval,exitflag,output] = fzero_fast(FunFcnIn,x,options,varargin)

fcount = 0;
iter = 0;
intervaliter = 0;
exitflag = 1;
procedure = ' ';

defaultopt = struct('Display','notify','TolX',eps,'FunValCheck','off','OutputFcn',[],'PlotFcns',[]);

% initialization
if nargin < 3, 
   options = []; 
end
% Detect problem structure input
if nargin == 1
    if isa(FunFcnIn,'struct')
        [FunFcnIn,x,options] = separateOptimStruct(FunFcnIn);
    else % Single input and non-structure.
        error(message('MATLAB:fzero:InputArg'));
    end
end

if nargin == 0 
    error(message('MATLAB:fzero:NotEnoughInputs')); 
end

% Check for non-double inputs
if~isa(x,'double')
  (message('MATLAB:fzero:NonDoubleInput'))
end

tol = optimget(options,'TolX',defaultopt,'fast');
funValCheck = strcmp(optimget(options,'FunValCheck',defaultopt,'fast'),'on');
printtype = optimget(options,'Display',defaultopt,'fast');

trace = 0;
haveoutputfcn = false;
plotfcns = [];
haveplotfcn = false;

FunFcn= FunFcnIn;

% Interval input
if (numel(x) == 2) 
    a = x(1); savea=a;
    b = x(2); saveb=b;
    % Put first feval in try catch

    fa = FunFcn(a,varargin{:});
    fb = FunFcn(b,varargin{:});

    if any(~isfinite([fa fb])) || any(~isreal([fa fb]))
        error(message('MATLAB:fzero:ValuesAtEndPtsComplexOrNotFinite'))
    end

    fcount = fcount + 2;
    savefa = fa; savefb = fb;
    
    if ( fa == 0 )
        b = a;
        msg = getString(message('MATLAB:fzero:ZeroFindTerminated'));

        output.intervaliterations = intervaliter;
        output.iterations = iter;
        output.funcCount = fcount;
        output.algorithm = 'bisection, interpolation';
        output.message = msg;
        fval = fa;
        return
    elseif ( fb == 0)
        % b = b;
        msg = getString(message('MATLAB:fzero:ZeroFindTerminated'));

        output.intervaliterations = intervaliter;
        output.iterations = iter;
        output.funcCount = fcount;
        output.algorithm = 'bisection, interpolation';
        output.message = msg;
        fval = fb;
        return
    elseif (fa > 0) == (fb > 0)
        error(message('MATLAB:fzero:ValuesAtEndPtsSameSign'))
    end
    
    % Starting guess scalar input
elseif (numel(x) == 1)
    error(message('Only interval input is supported'));
end % if (numel(x) == 2)

fc = fb;
procedure = 'initial';

% Main loop, exit from middle of the loop
while fb ~= 0 && a ~= b
    % Insure that b is the best result so far, a is the previous
    % value of b, and c is on the opposite side of the zero from b.
    if (fb > 0) == (fc > 0)
        c = a;  fc = fa;
        d = b - a;  e = d;
    end
    if abs(fc) < abs(fb)
        a = b;    b = c;    c = a;
        fa = fb;  fb = fc;  fc = fa;
    end
    
    % Convergence test and possible exit
    m = 0.5*(c - b);
    toler = 2.0*tol*max(abs(b),1.0);
    if (abs(m) <= toler) || (fb == 0.0) 
        break
    end
    
    % Choose bisection or interpolation
    if (abs(e) < toler) || (abs(fa) <= abs(fb))
        % Bisection
        d = m;  e = m;
    else
        % Interpolation
        s = fb/fa;
        if (a == c)
            % Linear interpolation
            p = 2.0*m*s;
            q = 1.0 - s;
        else
            % Inverse quadratic interpolation
            q = fa/fc;
            r = fb/fc;
            p = s*(2.0*m*q*(q - r) - (b - a)*(r - 1.0));
            q = (q - 1.0)*(r - 1.0)*(s - 1.0);
        end;
        if p > 0, q = -q; else p = -p; end;
        % Is interpolated point acceptable
        if (2.0*p < 3.0*m*q - abs(toler*q)) && (p < abs(0.5*e*q))
            e = d;  d = p/q;
        else
            d = m;  e = m;
        end;
    end % Interpolation
    
    % Next point
    a = b;
    fa = fb;
    if abs(d) > toler, b = b + d;
    elseif b > c, b = b - toler;
    else b = b + toler;
    end
    fb = FunFcn(b,varargin{:});
    fcount = fcount + 1;
    iter = iter + 1;
end % Main loop

fval = fb; % b is the best value


output.intervaliterations = intervaliter;
output.iterations = iter;
output.funcCount = fcount;
output.algorithm = 'bisection, interpolation';

if abs(fval) <= max(abs(savefa),abs(savefb))
    msg = sprintf(getString(message('MATLAB:fzero:ZeroFoundInInterval',sprintf('%g',savea),sprintf('%g',saveb))));
else
    exitflag = -5; 
    msg = sprintf(...
        getString(message('MATLAB:fzero:CurrentPointXNearSingularPoint',...
        sprintf('%g',savea),sprintf('%g',saveb))));
end



%------------------------------------------------------------------

function [exitflag,msg] = disperr(y, fy, trace)
%DISPERR Display an appropriate error message when FY is Inf, 
%   NaN, or complex.  Assumes Y is the value and FY is the function 
%   value at Y. If FY is neither Inf, NaN, or complex, it generates 
%   an error message.

if ~isfinite(fy)  % NaN or Inf detected
    exitflag = -3;
    msg = ...
        getString(message('MATLAB:fzero:AbortingSearchForIntervalNaNInfEncountered',sprintf('%g',y),sprintf('%g',fy)));
    if trace > 0
        disp(msg)
    end
elseif ~isreal(fy) % Complex value detected
    exitflag = -4;
    msg = ...
        getString(message('MATLAB:fzero:AbortingSearchForIntervalComplexEncountered',sprintf('%g',y),num2str(fy)));
    if trace > 0
        disp(msg)        
    end
elseif ~isfinite(y) % Inf detected in bracketting stage
    exitflag = -6;
    msg = ...
        getString(message('MATLAB:fzero:ExitingNoSignChange'));
    if trace > 0
        disp(msg)        
    end

else
    error(message('MATLAB:fzero:disperr:InvalidArg'))
end

%--------------------------------------------------------------------------
function [xOutputfcn, optimValues, stop] = callOutputAndPlotFcns(outputfcn,plotfcns,x,state,fcount,iter,intervaliter,  ...
    f,procedure,a,fvala,b,fvalb,varargin)
% CALLOUTPUTANDPLOTFCNS assigns values to the struct OptimValues and then calls the
% outputfcn/plotfcns.  
%
% state - can have the values 'init','iter', or 'done'. 
% We do not handle the case 'interrupt' because we do not want to update
% xOutputfcn or optimValues (since the values could be inconsistent) before calling
% the outputfcn; in that case the outputfcn/plotfcns are called directly rather than
% calling it inside callOutputAndPlotFcns.

% For the 'done' state we do not check the value of 'stop' because the
% optimization is already done.
optimValues.funccount = fcount;
optimValues.iteration = iter;
optimValues.intervaliteration = intervaliter;
optimValues.fval = f;
optimValues.procedure = procedure;
optimValues.intervala = a;
optimValues.fvala = fvala;
optimValues.intervalb = b;
optimValues.fvalb = fvalb;

xOutputfcn = x;  % set xOutputfcn to be x
stop = false;
% Call output functions
if ~isempty(outputfcn)
    switch state
        case {'iter','init'}
            stop = callAllOptimOutputFcns(outputfcn,xOutputfcn,optimValues,state,varargin{:}) || stop;
        case 'done'
            callAllOptimOutputFcns(outputfcn,xOutputfcn,optimValues,state,varargin{:});
        otherwise
            error(message('MATLAB:fzero:InvalidState'))
    end
end
% Call plot functions
if ~isempty(plotfcns)
    switch state
        case {'iter','init'}
            stop = callAllOptimPlotFcns(plotfcns,xOutputfcn,optimValues,state,varargin{:}) || stop;
        case 'done'
            callAllOptimPlotFcns(plotfcns,xOutputfcn,optimValues,state,varargin{:});
        otherwise
            error(message('MATLAB:fzero:InvalidState'))
    end
end

%--------------------------------------------------------------------------
function [b,fval,exitflag,output] = cleanUpInterrupt(xOutputfcn,optimValues)
% CLEANUPINTERRUPT updates or sets all the output arguments of FMINBND when the optimization 
% is interrupted.

% Call plot function driver to finalize the plot function figure window. If
% no plot functions have been specified or the plot function figure no
% longer exists, this call just returns.
callAllOptimPlotFcns('cleanuponstopsignal');

b = xOutputfcn;
fval = optimValues.fval;
exitflag = -1; 
output.intervaliterations = optimValues.intervaliteration;
output.iterations = optimValues.iteration;
output.funcCount = optimValues.funccount;
output.algorithm = 'bisection, interpolation';
output.message = getString(message('MATLAB:fzero:OptimizationTerminatedPrematurelyByUser'));

%--------------------------------------------------------------------------
function f = checkfun(x,userfcn,varargin)
% CHECKFUN checks for complex or NaN results from userfcn.

f = userfcn(x,varargin{:});
% Note: we do not check for Inf as FZERO handles it naturally.  ???
if isnan(f)
    error(message('MATLAB:fzero:checkfun:NaNFval', localChar( userfcn ), sprintf( '%g', x )));  
elseif ~isreal(f)
    error(message('MATLAB:fzero:checkfun:ComplexFval', localChar( userfcn ), sprintf( '%g', x )));  
end

%--------------------------------------------------------------------------
function strfcn = localChar(fcn)
% Convert the fcn to a string for printing

if ischar(fcn)
    strfcn = fcn;
elseif isa(fcn,'inline')
    strfcn = char(fcn);
elseif isa(fcn,'function_handle')
    strfcn = func2str(fcn);
else
    try
        strfcn = char(fcn);
    catch ME
        strfcn = getString(message('MATLAB:fzero:NameNotPrintable'));
    end
end
