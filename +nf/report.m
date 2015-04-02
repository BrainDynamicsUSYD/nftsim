function report(obj)
    % Given a nf output object, print some information about it
    % Romesh Abeysuriya 120322
    fprintf(1,'NeuroField output\n')
    fprintf(1,'Traces: ');
    for j = 1:length(obj.fields)
        fprintf(1,'%s ',obj.fields{j});
    end
    fprintf(1,'\nStart time: %.2fs\nStop time: %.2fs\ndeltat: %.2gs\nnpoints: %d\n',obj.time(1),obj.time(end),obj.deltat,obj.npoints);
    fprintf(1,'Nodes: %s\n',num2str(obj.nodes{1}));
end
