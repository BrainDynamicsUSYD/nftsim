function eegconfrestart2(iname,oname,whichpar,val)
%  eegconfrestart(iname,oname,whichpar,val)
% Load dumpfile iname, replace whichpar val, write to oname
% assumes ssepconf format, and whichpar='stimamp'|'stimfreq'

infid=fopen(iname);
outfid=fopen(oname,'w');

switch whichpar
    case 'stimamp'
        intest='Stimulus mode:3';
        instring='Stimulus mode:%d Time to start of stimulus:%g Amplitude:%g Modulation frequency :%g Propagation data\n';
        outstring='Stimulus mode:%d Time to start of stimulus:%g Amplitude:%g Modulation Frequency:%g Propagation data\n';
        pars_i=3;  % 3rd element along
    case 'stimfreq'
        intest='Stimulus mode:3';
        instring='Stimulus mode:%d Time to start of stimulus:%g Amplitude:%g Modulation frequency :%g Propagation data\n';
        outstring='Stimulus mode:%d Time to start of stimulus:%g Amplitude:%g Modulation Frequency:%g Propagation data\n';
        pars_i=4;  % 4th element along
    otherwise
        error('invalid whichpar')
end

while feof(infid)==0,
    tline=fgets(infid);
    if isempty(strfind(tline,intest))
        fprintf(outfid,tline);
    else
        pars=sscanf(tline,instring);
        pars(pars_i)=val;
        fprintf(outfid,outstring,pars);
    end
end

fclose(infid);
fclose(outfid);