@echo off

if not exist w:\build mkdir w:\build
pushd w:\build

set CompilerFlags=%1 -MTd -nologo -Oi -W4 -MP -Z7 -FC -EHsc -Iw:\deps\ -Iw:\src\ -Feviz
set LinkerFlags=/NODEFAULTLIB:MSVCRT /SUBSYSTEM:console /LIBPATH:w:\deps\SDL2\lib\ SDL2.lib SDL2main.lib opengl32.lib user32.lib gdi32.lib opengl32.lib kernel32.lib
set Files=w:\src\build.cpp w:\src\gl.cpp w:\deps\glad\glad.c w:\deps\imgui\imgui_compile.cpp

cl %CompilerFlags% %Files% /link %LinkerFlags%

xcopy w:\deps\SDL2\lib\SDL2.dll w:\build\ /C /Y  > NUL 2> NUL

popd
