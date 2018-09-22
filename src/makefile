
all:
	clang++ -msse4.1 -O2 -g3 build.cpp gl.cpp ../deps/glad/glad.c ../deps/imgui/imgui_compile.cpp -I../deps/ -I. -o viz -lGL -lSDL2 -ldl

clean:
	rm -rf *.o viz
