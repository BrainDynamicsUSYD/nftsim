function [Jee,Jei,Jese,Jesre,Jsrs,Jesn] = get_jabcd(self,w,x,y)
    % Retrieve the Jabcd values. These values are either scalars, or matrices

    % First, go through and calculate the L's

    assert(isempty(self.spatial_xyz)); % This function is not supposed to run if spatial XYZ are defined

    [L,M] = self.get_LM(w,x,y);
    
    % It is important to use the G_ab if they are available
    % because the G_ab define a value for G_sn which allows the
    % normalization with NeuroField to be correct.
    % The 5 gains do not specify the normalization of the power spectrum

    % Go through and decide 
    % Gab   Gabcd   Spatial Gab     Spatial Gabcd   
    %                                               Error
    % x                                             use gab
    %       x                                       use gabcd
    %                       x                       error
    %                               x               error
    % x     x                                       use gab
    % x                     x                       use gab
    % x                             x               error
    %       x               x                       error
    %       x                       x               use gabcd
    %                       x       x               error
    % x     x               x                       use gab
    % x     x                       x               use gabcd
    % x                     x       x               error
    %       x               x       x               error
    % x     x               x       x               error

    d = ~([isempty(self.gab) isempty(self.gabcd) isempty(self.spatial_gab) isempty(self.spatial_gabcd)]);
    if ~d(1) && ~d(2)
        error('No Gab specified')
    elseif d(3) && d(4)
        error('Spatial nonuniformities in both Gab and Gabcd detected')
    elseif all(d == [1 0 0 1]) || all(d == [0 1 1 0])
        error('Bad combination of parameters and spatial variations provided')
    elseif all(d == [1 0 0 0]) || all(d == [1 1 0 0]) || all(d == [1 0 1 0]) || all(d == [1 1 1 0])
        use_gabcd = false;
    elseif all(d == [0 1 0 0]) || all(d == [0 1 0 1]) || all(d == [1 1 0 1])
        use_gabcd = true;
    else
        error('Undefined combination of parameters!!')
    end


    if use_gabcd
        if size(self.gabcd,1) > 1
            error('More than one set of Gabcd provided!');
        end

        g = cell(1,5);
        for j = 1:5
            if ~isempty(self.spatial_gabcd) && isfunction(self.spatial_gabcd{j})
                g{j} = self.spatial_gabcd{j}(x,y);
            else
                g{j} = self.gabcd(j)*ones(size(x));
            end
        end

        Jee = bsxfun(@times,L{1},g{1});
        Jei = bsxfun(@times,L{2},g{2});
        Jese = bsxfun(@times,L{3}.*L{4}.*M.*M,g{3});
        Jesre = bsxfun(@times,L{3}.*L{5}.*L{7}.*M.*M,g{4});
        Jsrs = bsxfun(@times,L{5}.*L{8},g{5});
        Jesn = L{3}.*L{6}.*M;
    else
        if size(self.gab,1) > 1
            error('More than one set of Gab provided');
        end

        g = cell(1,8);
        for j = 1:8
            if ~isempty(self.spatial_gab) && isfunction(self.spatial_gab{j})
                g{j} = self.spatial_gab{j}(x,y);
            else
                g{j} = self.gab(j)*ones(size(x));
            end
        end

        Jee = bsxfun(@times,L{1},g{1});
        Jei = bsxfun(@times,L{2},g{2});
        Jes = bsxfun(@times,L{3}.*M,g{3});
        Jse = bsxfun(@times,L{4}.*M,g{4});
        Jsr = bsxfun(@times,L{5},g{5});
        Jsn = bsxfun(@times,L{6},g{6});
        Jre = bsxfun(@times,L{7}.*M,g{7});
        Jrs = bsxfun(@times,L{8},g{8});

        Jese = Jes.*Jse;
        Jesre = Jes.*Jsr.*Jre;
        Jsrs = Jsr.*Jrs;
        Jesn = Jes.*Jsn;
    end
