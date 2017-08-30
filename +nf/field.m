%% Returns field name according to the pre-/postfix and array of indices.
%
% ARGUMENTS:
%        prefix -- .
%        index -- .
%        postfix -- .
%
% OUTPUT:
%        ret -- .
%
% AUTHOR:
%     Romesh Abeysuriya (2012-03-22).
%
% USAGE:
%{
    %
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function ret = field(prefix, index, postfix)
    ret = '';
    for ii = 1:length(index)
        if ~strcmp(ret, '')
            ret = [ret, ','];
        end
        ret = [ret, prefix, '.', num2str(index(ii)), '.', postfix];
    end
end %function field()
