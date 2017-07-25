%% Given a neurofield output struct, print some information about it.
%
% ARGUMENTS:
%        obj -- A neurofield output struct (a Matlab struct containing data
%               from a simulation).
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
    fprintf(1, 'Output for: neurofield -i "%s"\n', obj.conf_file)
    fprintf(1, 'Traces: ');
    for j = 1:length(obj.fields)
        fprintf(1, '%s ', obj.fields{j});
    end
    fprintf(1, '\n')
    fprintf(1, 'Start time: %.2fs\n', obj.time(1));
    fprintf(1, 'Stop time: %.2fs\n', obj.time(end));
    fprintf(1, 'Time Step: %.2gs\n', obj.deltat);
    fprintf(1, 'Time points: %d\n', obj.npoints);
    fprintf(1, 'Nodes: %s\n', num2str(obj.nodes{1}));
end %function report()
