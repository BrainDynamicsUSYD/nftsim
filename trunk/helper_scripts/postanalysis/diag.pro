;
;  Code snippet for import of data into IDL.
;
set_plot, "ps"
; A code snippet to read Phiout data from EEGcode
;
;
openr,1,'eegcode.output'
;
; Read preliminary data from eegcode.output file
;
readf, 1, deltat, FORMAT='(8X,G10)'
print, deltat
readf, 1, steps, FORMAT='(28X,I10)'
print, steps
readf, 1, traces, FORMAT='(31X,I10)'
print, traces
for i=1, traces do begin
  readf, 1, dummy, FORMAT='(62X,I10)' ; Skip the node specification lines
endfor
;
;       
;
;
time=findgen(steps) ; These two lines create time array
time=deltat*time
;
plt=dblarr(traces,steps)
readf, 1, plt ; Read the Phi arrays in
;
;  First trace
;
plot, time, plt[0,*], xtitle="time (s)", ytitle="!7u!3", YRANGE=[0.2,20.] ;
;
;  Plot second trace
;
for i=1, (traces-1) do begin
  plot, time, plt[i,*]
endfor
;
; Divide into 2 sec increments which are Fourier transformed to obtain
; power spectra. The power spectra are then averaged to obtain an average
; power spectrum. Note that first 1 sec of data which may have transients
; has to be discarded
;
;
; Calculate number of power spectra and points in each spectrum
;
numspectra= LONG((steps*deltat-1.0)/2.0) ; Note first second is effectively discarded
numptsinspectra= LONG(2.0/deltat)
for k=0, (traces-1) do begin
;
; Calculate individual spectra and average them
;
powerspectra=fltarr(numptsinspectra)
powerspectra[*]=0.
for i=0, (numspectra-1) do begin
  removezero=MEAN(plt[k,((LONG(i)+0.5)*numptsinspectra):((LONG(i+1)+0.5)*numptsinspectra-1)])
  plt[k,((LONG(i)+0.5)*numptsinspectra):((LONG(i+1)+0.5)*numptsinspectra-1)]-=removezero
  array = ABS(FFT(plt[k,((LONG(i)+0.5)*numptsinspectra):((LONG(i+1)+0.5)*numptsinspectra-1)]))^2
  powerspectra=powerspectra+array
endfor
powerspectra=powerspectra/numspectra
;
; Zero out the constant (f=0) point and throw away negative frequencies
; and double the spectrum
;
powerspectra[0]=0.
powerspectra=2.0*powerspectra[0:(numptsinspectra/2)]
;
; Calculate the frequency axis for the power spectrum
;
freq=findgen(numptsinspectra/2)
freq=freq/(deltat*numptsinspectra)
;
; Plot the power spectrum
;
;
plot_oo, freq, powerspectra, xrange=[1,2000], yrange=[10E-12,0.01]
noisefloor=fltarr(numptsinspectra/2)
noisefloor[*]=powerspectra[1]*10E-7
oplot, freq, noisefloor, linestyle=2
endfor
close, 1
device, /close
END
