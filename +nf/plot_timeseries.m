%% Plots time-series formatted like a typical EEG view.
%
%  That is, nodes are displayed with vertical space between lines. By
%  default, for each time-step the spatial average is removed -- this
%  corresponds to a rereferencing to the "grand average". One figure window
%  is created for each trace.
%
% ARGUMENTS:
%    obj -- A Matlab struct containing data from a simulation as returned
%           by nf.read().
%    traces   -- cell array with the labels (strings) of the traces to use.
%    node_idx -- cell array with vectors of node indices, one per trace.
%    reref -- boolean, true: removes spatial average at each time-step,
%             this is equivalent to the "grand-average" often used in EEG.
%
% OUTPUT: (Generates a time-series figure for each trace.)
%    figure_handles -- cell array of figure handles.
%
% REQUIRES:
%    nf.extract() -- Extract a specific subset of data from a nftsim
%                    output struct.
%
% AUTHOR:
%    Stuart A. Knock (https://github.com/stuart-knock/BrainNetworkModels/blob/master/PlottingTools/PlotTimeSeries.m)
%    Paula Sanz-Leon
%
% USAGE:
%{
    %Either run a simulation:
    nf_obj = nf.run('./configs/eirs-corticothalamic.conf')
    %Or load some nftsim output data
    nf_obj = nf.read('./configs/eirs-corticothalamic.output')

    %Plot every fourth node for the trace 'Propagator.1.phi'.
    figure_handles = nf.plot_timeseries(nf_obj, {'Propagator.1.phi'}, {1:4:144})

    %All figures just generated can be closed using:
    close([figure_handles{:}])
%}

function figure_handles =  plot_timeseries(obj, traces, node_idx, reref)
  % Default to plotting all fields.
  if nargin < 2 || isempty(traces)
    traces = obj.fields;
  end
  % Default to plotting the first 64 nodes.
  if nargin < 3 || isempty(node_idx)
    node_idx = cell(1, length(traces));
    for j = 1:length(traces)
      node_idx{j} = 1:64;
    end
  end
  % Default to rereferencing to the "grand average".
  if nargin < 4
    reref = true;
  end
  % Get some size info about data
  num_figs   = length(traces);
  time       = obj.time;
  time_steps  = length(time);

  figure_handles = cell(1, num_figs);
  for nof=1:num_figs
    num_nodes = length(node_idx{nof});
    labels = cell(1, num_nodes);
    for k = 1:num_nodes
        labels{k} = num2str(node_idx{nof}(k)); 
    end %index labels

    figure_handles{nof} = figure;
    fig_title = [traces{nof}];
    % Get fata to plot
    data = nf.extract(obj, traces{nof});
    data = detrend(data);

    % Rescale data to the range [0, 1]
    data = data - min(data(:)); 
    data = data ./ max(data(:));

    % Rereference to the "grand average". This is similar to the case 1 in
    % nf.movie, it removes the spatial average at each time frame.
    if reref
        datamean = mean(data, 2);
        data = bsxfun(@minus, data, datamean);
    end

    % Calculate teh line separation to use in the figure.
    separate_by = 0.33*(max(data(:)) - min(data(:)));
    nodes_by_timestepsize = repmat((1:num_nodes), [time_steps,1]);
    
    plot(time, data(:, node_idx{nof}) + separate_by*nodes_by_timestepsize, ...
         'color', [0.42, 0.42, 0.42]);
    
    title(fig_title, 'interpreter', 'none');
    xlabel('Time [s]');
    ylabel('Node index')
    set(gca, 'ylim', [0 separate_by*(num_nodes+1)]);
    set(gca, 'YTick', separate_by*(1:num_nodes));
    set(gca, 'YTickLabel', labels);
    clear labels
  end %for nof=1:num_figs

end %plot_timeseries
