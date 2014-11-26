function varargout = spectrum(p,verbose,draw_q2)
    %[f,P,q2re2] = analytic_spectrum(p,nonlinear,verbose,draw_q2)
    % To make a plot, analytic_spectrum(p) 
    % To get data, [f,P] = analytic_spectrum(p);
    % verbose = 1 means to make a plot
    % verbose = 2 prints parameters too

    if nargin < 3 || isempty(draw_q2)
        draw_q2 = 0;
    end
    
    if (nargin < 2 || isempty(verbose)) && nargout == 0
        verbose = 1;
    elseif nargin < 2 || isempty(verbose)
        verbose = 0;
    end
       
    if draw_q2
    	[~,f,P,w,q2] = p.dispersion_calc(0);
    	q2 = q2.*p.re^2;
    else
    	[~,f,P] = p.dispersion_calc(1);
	end
    
    if verbose
        if ishandle(verbose)
            figure(verbose)
        else
            figure(1)
        end

        h(1) = loglog(f,P,'g'); % Plot of the power spectrum
	    set(gca,'XLim',[1 100]);
	    hold on
	
	    bands = model.eeg_bands;
	    ylims = get(gca,'YLim');

	    for j = unique(bands([1:3,5:6],:))
	    	plot([j,j],[ylims(1) ylims(2)],'b--');
	    end

	    plot([bands(4,1),bands(4,1)],[ylims(1) ylims(2)],'r--');
	    plot([bands(4,2),bands(4,2)],[ylims(1) ylims(2)],'r--');

	    text((bands(1,2)+bands(1,1))/2,ylims(1)*1e1,'Delta','HorizontalAlignment','center')
	    text((bands(2,2)+bands(2,1))/2,ylims(1)*1e1,'Theta','HorizontalAlignment','center')
	    text((bands(3,2)+bands(3,1))/2,ylims(1)*1e1,'Alpha','HorizontalAlignment','center')
	    text((bands(4,2)+bands(4,1))/2,ylims(1)*5e1,'Sigma','Color','r','HorizontalAlignment','center')
	    text((bands(5,2)+bands(5,1))/2,ylims(1)*1e1,'Beta','HorizontalAlignment','center')

	    if verbose == 2
		    if isempty(findobj('userdata','parameter_values'))
		    	print_params(process(p),'ne');
		    end
		end

        if draw_q2
	        figure
	        plot3(real(q2),imag(q2),w)
	        hold on
	        plot([-1e6 1e6],[0,0],'k--','XLimInclude','off')
	        plot([0,0],[-1e6 1e6],'k--','YLimInclude','off')
	        xlabel('Re(q^2)');
	        ylabel('Im(q^2)');
	        set(gca,'View',[0 90]);
        end

	end





	if nargout > 0
		varargout{1} = f;
		varargout{2} = P;
		if draw_q2
			varargout{3} = q2;
		end
	end
end

function p = process(p)
	% Remove unwanted fields from the parameters before printing
	p = rmfield(p,{'taues','tause','gammas','rs','gamman','rn','gabcd','ve','nroots','ves'});
end