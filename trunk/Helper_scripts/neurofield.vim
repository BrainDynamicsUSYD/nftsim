" neurofield.vim
" implements syntax highlighting for *.conf in vi
" to install, copy this file into ~/.vim/syntax/
" and add "au BufRead,BufNewFile *.conf setfiletype neurofield" into
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
" A "Special" number
syn keyword number Steady All

syn keyword key Connection matrix Population Stimulus Population Firing Dendrite Propag Couple Output nextgroup=index skipwhite skipnl skipempty
syn match index '\d\+' contained

syn keyword choose Composite Const White CoherentWhite Pulse Sine Gaussian Ramp GaussPulse MNS Sigmoid Linear Map Wave Harmonic CaDP Q V phi Tau nu

syn keyword header Time Deltat Nodes Glutamate dynamics

syn keyword cntmatc From To nextgroup=cntmati skipwhite skipnl skipempty
syn match cntmati '\d\+' contained

"hi def link object Type
hi def link key Type
hi def link index Type
hi def link choose Statement
hi def link header PreProc
hi def link number Constant
hi def link cntmatc PreProc
hi def link cntmati PreProc

let b:current_syntax = "neurofield"
