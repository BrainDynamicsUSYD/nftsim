function nf_report(nf)
    % Given a nf output object, print some information about it
    % Romesh Abeysuriya 120322
    fprintf(1,'NEUROFIELD OUTPUT INFORMATION\n')
    fprintf(1,'Traces: ');
    for j = 2:length(nf.fields)
        fprintf(1,'%s ',nf.fields{j});
    end
    fprintf(1,'\nStart time: %.2fs\nStop time: %.2fs\ndeltat: %.2gs\nnpoints: %d\n',nf.time(1),nf.time(end),nf.time(2)-nf.time(1),length(nf.time));
    fprintf(1,'Nodes: %s\n',num2str(nf.nodes{2}));
end
