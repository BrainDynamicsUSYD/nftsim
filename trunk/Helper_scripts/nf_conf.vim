" nf_conf.vim
" implements syntax highlighting for *.conf in vi
" to install, copy this file into ~/.vim/syntax/
" and add "au BufRead,BufNewFile *.conf setfiletype nf_conf" into
" ~/.vim/filetype.vim

" Integer with - + or nothing in front
syn match number '\<\d\+\>'
syn match number '\<[-+]\d\+\>'
" Floating point number with decimal no E or e (+,-)
syn match number '\<\d\+\.\d*\>'
syn match number '\<[-+]\d\+\.\d*\>'
" Floating point like number with E and no decimal point (+,-)
syn match number '\<[-+]\=\d[[:digit:]]*[eE][\-+]\=\d\+\>'
syn match number '\<\d[[:digit:]]*[eE][\-+]\=\d\+\>'
" Floating point like number with E and decimal point (+,-)
syn match number '\<[-+]\=\d[[:digit:]]*\.\d*[eE][\-+]\=\d\+\>'
syn match number '\<\d[[:digit:]]*\.\d*[eE][\-+]\=\d\+\>'

syn keyword constant Steady All Torus Nonperiodic

syn keyword type Connection matrix Population Stimulus Population Firing Dendrite Propag Couple Output nextgroup=index skipwhite skipnl skipempty
syn match index '\d\+' contained

syn keyword statement Composite Const White CoherentWhite Pulse Sine Gaussian Ramp GaussPulse PAS Sigmoid Linear Bursting Map Wave Harmonic CaDP BCM STP Multi Q V phi Tau nu

syn keyword preproc Time Deltat Nodes Topology Bath Glutamate dynamics

syn keyword cntmatc From To nextgroup=cntmati skipwhite skipnl skipempty
syn match cntmati '\d\+' contained

syn region error start="\%^" end="$"

"hi def link object Type
hi def link index Type
hi def link cntmatc PreProc
hi def link cntmati PreProc

let b:current_syntax = "neurofield"
