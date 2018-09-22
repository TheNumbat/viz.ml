
#pragma once

#define MATH_NO_IMPLEMENTATIONS
#include <vmath.h>
#include <glad/glad.h>
#include <string>
#include <vector>

const f32 q_vbo_data[] = {
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
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

struct scene {
	std::vector<v3> 	mesh_vertices;
	std::vector<uv3> 	mesh_elements;
	
	std::vector<v4> 	i_positions;
	std::vector<colorf> i_colors;

	GLuint fbo = 0, id_fbo = 0;
	GLuint texout[2] = {}, id_texout = 0;
	GLuint rbo = 0, id_rbo = 0;

	GLuint vao = 0;
	GLuint m_vbo[2] = {}, i_vbo[2] = {};
	bool m_dirty = false, i_dirty = false;

	GLuint q_vao = 0, q_vbo = 0;

	shader s_shader, q_shader;

	i32 w, h;

	void init(int w, int h);
	void init_fbo();
	void update_wh(int w, int h);
	void update();
	void clear();
	void render(m4 transform);
	void destroy();
	void destroy_fbo();
	i32  read_id(i32 x, i32 y);

	void push_sphere(v3 pos, f32 r);
	void add_data(v3 pos, colorf c, i32 id);
};

void debug_proc(GLenum glsource, GLenum gltype, GLuint id, GLenum severity, GLsizei length, const GLchar* glmessage, const void* up);
void gl_check_leaked_handles();
