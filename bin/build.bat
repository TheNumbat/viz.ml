@echo off

if not exist w:\build mkdir w:\build
pushd w:\build

set CompilerFlags=%1 -MTd -nologo -Oi -W4 -MP -Z7 -FC -EHsc -Iw:\deps\ -Iw:\src\ -Feviz -wd4100
set LinkerFlags=/NODEFAULTLIB:MSVCRT /SUBSYSTEM:windows /LIBPATH:w:\deps\SDL2\lib\ /LIBPATH:w:\deps\nlopt\lib\ SDL2.lib SDL2main.lib nlopt.lib opengl32.lib user32.lib gdi32.lib opengl32.lib kernel32.lib
set Files=w:\src\main.cpp w:\src\gl.cpp w:\deps\glad\glad.c w:\deps\imgui\imgui_compile.cpp 

set Extra=w:\deps\bhtsne\sp_tsne.cpp w:\deps\bhtsne\sptree.cpp

cl %CompilerFlags% %Files% /link %LinkerFlags%

xcopy w:\deps\SDL2\lib\SDL2.dll w:\build\ /C /Y  > NUL 2> NUL
xcopy w:\deps\nlopt\lib\nlopt.dll w:\build\ /C /Y  > NUL 2> NUL

popd
