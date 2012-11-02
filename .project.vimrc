" Some useful VIM stuff for developing Swift
"
" Add the following in your ~/.vimrc to automatically load this when
" you open vim in the toplevel directory:
"   
"   if filereadable(".project.vimrc")
"     source .project.vimrc
"   endif
"

" CPPUnit QuicFix support
set errorformat+=%E%.%#\ test:\ %.%#line:\ %l\ %f,%Z%m

" SCons support
set makeprg=python\ 3rdParty/SCons/scons.py\ check=1

set noexpandtab

let g:syntastic_c_include_dirs = [ '.', 'Swift/QtUI', '3rdParty/Boost/src' ]
