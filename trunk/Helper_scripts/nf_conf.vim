" nf_conf.vim
" implements syntax highlighting for *.conf in vi
" to install, copy this file into ~/.vim/syntax/
" and add "au BufRead,BufNewFile *.conf setfiletype nf_conf" into
" ~/.vim/filetype.vim

syn region error start='\%^' end='$'

syn keyword preproc Time Deltat Nodes Topology Bath Glutamate dynamics Onset Cease Duration Output

syn keyword CntKey From To nextgroup=CntI skipwhite skipnl skipempty
syn match CntI '\d\+' contained

syn keyword Object Connection matrix Population Stimulus Population Firing Dendrite Propag Couple nextgroup=Index skipwhite skipnl skipempty
syn match Index '\d\+' contained

syn match Choice /:\D*-/ms=s+1,me=e-1
syn keyword statement Q phi Tau nu

syn match Number '\<[-+]\=\d*\.\=\d*[eE]\=[-+]\=\d*\>'
syn keyword Constant Steady All Torus Nonperiodic

hi def link Object type
hi def link CntKey preproc
hi def link CntI preproc
hi def link Choice statement

let b:current_syntax = "neurofield"
