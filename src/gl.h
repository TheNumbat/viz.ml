
#pragma once

#define MATH_NO_IMPLEMENTATIONS
#include <vmath.h>
#include <glad/glad.h>
#include <string>
#include <vector>

struct scene {
	std::vector<v3> 	mesh_vertices;
	std::vector<uv3> 	mesh_elements;
	
	std::vector<v3> 	i_positions;
	std::vector<colorf> i_colors;

	GLuint vao = 0;
	GLuint m_vbo[2] = {}, i_vbo[2] = {};
	bool m_dirty = false, i_dirty = false;

	void init();
	void update();
	void clear();
	void render();
	void destroy();

	void push_sphere(v3 pos, f32 r);
	void add_data(v3 pos, colorf c);
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
