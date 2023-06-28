set PROFILING= 
rem set PROFILING=--profiling 

emcc -std=c++11 -v -O3 %PROFILING% --bind --memory-init-file 0 --llvm-opts 3 -w -v -s NO_EXIT_RUNTIME=1 --pre-js ds_pre.js  -I..\..\..\Common -I..\..\src\Values -I..\..\src\Graph -I..\..\src\Script -I..\..\src\Script\Modules -I..\..\src\Values -I..\..\src\Sys -I..\..\src\Platforms\JS -I..\..\src all_cpp_file.cpp -o ds_asm.js


