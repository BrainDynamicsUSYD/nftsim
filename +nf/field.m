function ret = field( prefix, index, postfix )
    % returns field name according to the pre-/postfix and array of indices
    ret = '';
    for ii = 1:length(index)
        if ~strcmp( ret,'' )
            ret = [ret,','];
        end
        ret = [ret, prefix,'.',num2str(index(ii)),'.',postfix];
    end
end
