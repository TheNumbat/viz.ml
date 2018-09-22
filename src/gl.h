
#pragma once

#define MATH_NO_IMPLEMENTATIONS
#include <vmath.h>
#include <glad/glad.h>
#include <string>
#include <vector>

struct mesh {
	std::vector<v3> 	vertices;
	std::vector<colorf> colors;
	std::vector<uv3> 	elements;

	GLuint vao = 0;
	GLuint v_vbo = 0, c_vbo = 0, ebo = 0;
	bool dirty = false;

	void init();
	void update();
	void clear();
	void render();
	void destroy();

	void push_cube(v3 pos, f32 len);
};

struct shader {
	
	GLuint program = 0;
	GLuint v = 0, f = 0;

	void use();
	GLuint getUniform(const GLchar* name);
	
	void destroy();
	void load(std::string vfile, std::string ffile);

	static void check(GLuint program);
};

void debug_proc(GLenum glsource, GLenum gltype, GLuint id, GLenum severity, GLsizei length, const GLchar* glmessage, const void* up);
void gl_check_leaked_handles();
