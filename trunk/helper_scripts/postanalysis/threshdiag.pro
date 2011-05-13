;
;  Code snippet for import of data into IDL.
;
set_plot, "ps"
; A code snippet to read Phiout preliminary data from EEGcode
;
;
;
; Find out how many lines are in file and assign to b
;
openr,1,'eegcode.threshout.1'
s=''
b=0L
while NOT EOF(1) do begin
   readf,1,s
   b=b+1
endwhile
close,1
;
openr,1,'eegcode.threshout.1'
;
; Skip Dummy lines
;
dummy=''
readf, 1, dummy ; Skip lines
readf, 1, dummy ; Skip lines
;
numheader=2 ;
numfields=3 ; number of lines of data
numrecs=(b-numheader)/(numfields+1) ; note 4 lines per record and two lines in header
print,numrecs
plt=dblarr(numfields,numrecs) 
readf, 1, plt;
;
plot, plt[0,*]
plot, plt[1,*]
plot, plt[2,*]
;
close, 1
device, /close
END

