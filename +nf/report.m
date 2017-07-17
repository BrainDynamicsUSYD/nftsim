%% Given a nf output object, print some information about it.
%
% ARGUMENTS:
%        obj -- nf object
%
% OUTPUT: Prints to terminal.
%
% AUTHOR:
%     Romesh Abeysuriya (2012-03-22).
%
% USAGE:
%{
    %
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function report(obj)
    fprintf(1, 'NeuroField output\n')
    fprintf(1, 'Traces: ');
    for j = 1:length(obj.fields)
        fprintf(1, '%s ', obj.fields{j});
    end
    fprintf(1, '\nStart time: %.2fs\nStop time: %.2fs\ndeltat: %.2gs\nnpoints: %d\n', obj.time(1), obj.time(end), obj.deltat, obj.npoints);
    fprintf(1, 'Nodes: %s\n', num2str(obj.nodes{1}));
end %function report()
