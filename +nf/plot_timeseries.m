%% Plots time-series, with vertical space between lines, one window 
% for each trace, like typical EEG view.
%
% ARGUMENTS:
%        obj -- A Matlab struct containing data
%               from a simulation as returned by nf.read()
%        traces    -- cell array with the labels (strings) of the traces to use
%        node_idx  -- cell array with vectors with node indices
%
% OUTPUT:
%        None
%
% REQUIRES:
%        
%        nf.extract() -- Extract a specific subset of data from a neurofield output struct.
%        
%
%
%
% OUTPUT: 
%           figure_handles -- cell array of figure handles.
% AUTHOR:
%     Stuart A. Knock 
%     Paula Sanz-Leon
%
% USAGE:
%{
    nf.plot_timeseries(nf_obj, {'Propagator.1.phi'}, {[8080:8120]})
%}

function figure_handles =  plot_timeseries(obj, traces, node_idx, scaling)

  % Get some size info about data
  num_figs   = length(traces);
  time       = obj.time;
  time_steps  = length(time);
  separate_by = zeros(1, num_figs);


  for nof=1:num_figs
    num_nodes = length(node_idx{nof});
    for k = 1:num_nodes
        labels{k} = num2str(node_idx{nof}(k)); 
    end %index labels

    figure_handles{nof} = figure(nof);
    fig_title = [traces{nof}];
    % Get fata to plot
    data = nf.extract(obj, traces{nof});
    data = detrend(data);

    % Rescale data to the range [0, 1]
    data = data - min(data(:)); 
    data = data ./ max(data(:));

    if scaling % this scaling is similar to the case 1 in nf.movie, it removes the spatial average at each time frame
        datamean = mean(data, 2);
        data = bsxfun(@minus, data, datamean);
    end

    separate_by(1,nof) = 0.33*(max(data(:)) - min(data(:)));
    nodes_by_timestepsize = repmat((1:num_nodes), [time_steps,1]);
    
    plot(time, data(:, node_idx{nof}) + separate_by(1,nof)*nodes_by_timestepsize, 'color', [0.42, 0.42, 0.42]); 
    
    title(fig_title, 'interpreter', 'none');
    xlabel('Time [s]');
    ylabel('Node index')
    set(gca,'ylim',[0 separate_by(1,nof)*(num_nodes+1)]);
    set(gca,'YTick', separate_by(1,nof)*(1:num_nodes));
    set(gca,'YTickLabel', labels);
    clear labels
  end

end %plot_timeseries