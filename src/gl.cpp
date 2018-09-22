
#include "gl.h"
#include <imgui/imgui.h>
#include <fstream>
#include <iostream>

void shader::destroy() {
	glUseProgram(0);
	glDeleteShader(v);
	glDeleteShader(f);
	glDeleteProgram(program);
	v = f = program = 0;
}

void shader::load(std::string vfile, std::string ffile) {
	
	std::string vs, fs;
	std::ifstream vfin(vfile), ffin(ffile);
	getline(vfin, vs, '\0');
	getline(ffin, fs, '\0');

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* vs_c = vs.c_str();
	const GLchar* fs_c = fs.c_str();
	glShaderSource(v, 1, &vs_c, NULL);
	glShaderSource(f, 1, &fs_c, NULL);
	glCompileShader(v);
	glCompileShader(f);

	check(v);
	check(f);

	program = glCreateProgram();
	glAttachShader(program, v);
	glAttachShader(program, f);
	glLinkProgram(program);
}

void shader::check(GLuint program) {

	GLint isCompiled = 0;
	glGetShaderiv(program, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE) {
		
		GLint len = 0;
		glGetShaderiv(program, GL_INFO_LOG_LENGTH, &len);

		char* msg = new char[len];
		glGetShaderInfoLog(program, len, &len, msg);

		std::cout << "Shader  " << program << " failed to compile: " << msg << std::endl;
		delete[] msg;
	}
}

void shader::use() {
	glUseProgram(program);
}

GLuint shader::getUniform(const GLchar* name) {
	return glGetUniformLocation(program, name);
}

void scene::init_fbo() {

	if(pixel_data) delete[] pixel_data;
	pixel_data = new f32[w * h * 3]();

	{
		glGenFramebuffers(1, &id_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, id_fbo);

		glGenTextures(1, &id_texout);

		glBindTexture(GL_TEXTURE_2D, id_texout);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenRenderbuffers(1, &id_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, id_rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id_texout, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, id_rbo);

		GLenum drawbufs[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, drawbufs);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	{
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glGenTextures(2, texout);

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texout[0]);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, NUM_SAMPLES, GL_RGB8, w, h, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texout[1]);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, NUM_SAMPLES, GL_RGB8, w, h, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, NUM_SAMPLES, GL_DEPTH_COMPONENT, w, h);  

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texout[0], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, texout[1], 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		GLenum drawbufs[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, drawbufs);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void scene::init(i32 _w, i32 _h) {

	w = _w; h = _h;

	s_shader.load("scene.v", "scene.f");
	q_shader.load("quad.v", "quad.f");

	push_sphere(v3(0, 0, 0), 1.0f);

	{
		glGenVertexArrays(1, &q_vao);
		glGenBuffers(1, &q_vbo);

		glBindVertexArray(q_vao);

		glBindBuffer(GL_ARRAY_BUFFER, q_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(q_vbo_data), &q_vbo_data, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	init_fbo();

	glGenVertexArrays(1, &vao);
	glGenBuffers(2, m_vbo);
	glGenBuffers(2, i_vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[1]);

	glBindBuffer(GL_ARRAY_BUFFER, i_vbo[0]);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);

	glBindBuffer(GL_ARRAY_BUFFER, i_vbo[1]);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);

	glBindVertexArray(0);
}

void scene::update_wh(int _w, int _h) {

	w = _w; h = _h;

	destroy_fbo();
	init_fbo();
}

void scene::destroy_fbo() {

	delete[] pixel_data;
	pixel_data = null;

	glDeleteRenderbuffers(1, &rbo);
	glDeleteTextures(2, texout);
	glDeleteFramebuffers(1, &fbo);

	glDeleteRenderbuffers(1, &id_rbo);
	glDeleteTextures(1, &id_texout);
	glDeleteFramebuffers(1, &id_fbo);

	fbo = texout[0] = texout[1] = rbo = id_texout = id_fbo = id_rbo = 0;
}

void scene::destroy() {

	destroy_fbo();

	s_shader.destroy();
	q_shader.destroy();

	glDeleteBuffers(2, m_vbo);
	glDeleteBuffers(2, i_vbo);
	glDeleteVertexArrays(1, &vao);

	glDeleteBuffers(1, &q_vbo);
	glDeleteVertexArrays(1, &q_vao);

	vao = m_vbo[0] = m_vbo[1] = i_vbo[0] = i_vbo[1] = 0;
	q_vbo = q_vao = 0;
}

void scene::clear() {
	i_positions.clear();
	i_colors.clear();
	i_dirty = true;
}

void scene::add_data(v3 pos, colorf c, i32 id) {

	i_positions.push_back(v4(pos, (f32)id));
	i_colors.push_back(c);
	i_dirty = true;
}

i32 scene::read_id(i32 x, i32 y) {

	y = h - y - 1;
	i32 idx = y * w * 3 + x * 3;
	
	i32 a = (i32)(pixel_data[idx] * 255.0f);
	i32 b = (i32)(pixel_data[idx + 1] * 255.0f);
	i32 c = (i32)(pixel_data[idx + 2] * 255.0f);

	i32 result = a | b << 8 | c << 16;

	return result;
}

void scene::update() {

	glBindVertexArray(vao);
	
	if(m_dirty) {

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(v3) * mesh_vertices.size(), mesh_vertices.size() ? &mesh_vertices[0] : null, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uv3) * mesh_elements.size(), mesh_elements.size() ? &mesh_elements[0] : null, GL_STATIC_DRAW);
		
		m_dirty = false;
	}

	if(i_dirty) {

		glBindBuffer(GL_ARRAY_BUFFER, i_vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colorf) * i_colors.size(), i_colors.size() ? &i_colors[0] : null, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, i_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(v4) * i_positions.size(), i_positions.size() ? &i_positions[0] : null, GL_DYNAMIC_DRAW);

		i_dirty = false;
	}

	glBindVertexArray(0);
}

void scene::render(m4 transform) {

	update();
	
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, w, h);
		glEnable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texout[0]);

		glClearColor(0.6f, 0.65f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		s_shader.use();
		glUniformMatrix4fv(s_shader.getUniform("transform"), 1, GL_FALSE, transform.a);

		glBindVertexArray(vao);
		glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)mesh_elements.size() * 3, GL_UNSIGNED_INT, (void*)0, (GLsizei)i_positions.size());
		glBindVertexArray(0);
	}

	{
		glBindFramebuffer(GL_FRAMEBUFFER, id_fbo);

		glViewport(0, 0, w, h);
		glDisable(GL_DEPTH_TEST);
		
		q_shader.use();
		glBindVertexArray(q_vao);
		
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texout[1]);
		glUniform1i(q_shader.getUniform("tex"), 0);
		glUniform1i(q_shader.getUniform("samples"), 1);
		glUniform2f(q_shader.getUniform("tex_size"), (f32)w, (f32)h);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, id_texout);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, pixel_data);
	}

	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, w, h);
		glDisable(GL_DEPTH_TEST);

		q_shader.use();
		glBindVertexArray(q_vao);
		
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texout[0]);
		glUniform1i(q_shader.getUniform("tex"), 0);
		glUniform1i(q_shader.getUniform("samples"), NUM_SAMPLES);
		glUniform2f(q_shader.getUniform("tex_size"), (f32)w, (f32)h);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
}

void scene::push_sphere(v3 pos, f32 r) {

	i32 divisions = 32;
	i32 p_divisions = divisions / 2 + 1;

	f32 th = 0.0f;
	for(i32 i = 0; i <= divisions; i++) {

		f32 ph = 0.0f;
		for(i32 j = 0; j < p_divisions; j++) {

			f32 ct = cos(th), st = sin(th), sp = sin(ph), cp = cos(ph);
			v3 point = v3(r * ct * sp, r * cp, r * st * sp);

			mesh_vertices.push_back(pos + point);

			ph += (PI32 * 2.0f) / divisions;
		}

		th += (PI32 * 2.0f) / divisions;
		if(i == divisions - 1) th += 0.0001f;
	}

	for (int x = 0; x < divisions; x++) {
		for (int y = 0; y < p_divisions - 1; y++) {
			GLuint idx = x * p_divisions + y;

			i32 idx1 = (idx + 1);
			i32 idxp = (idx + p_divisions);
			i32 idxp1 = (idx + p_divisions + 1);

			mesh_elements.push_back(uv3(idx, idx1, idxp));
			mesh_elements.push_back(uv3(idx1, idxp, idxp1));
		}
	}

	m_dirty = true;
}

void debug_proc(GLenum glsource, GLenum gltype, GLuint, GLenum severity, GLsizei, const GLchar* glmessage, const void*) {

	std::string message((char*)glmessage);
	std::string source, type;

	switch(glsource) {
	case GL_DEBUG_SOURCE_API:
		source = "OpenGL API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		source = "Window System";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		source = "Shader Compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		source = "Third Party";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		source = "Application";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		source = "Other";
		break;
	}

	switch(gltype) {
	case GL_DEBUG_TYPE_ERROR:
		type = "Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		type = "Deprecated";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		type = "Undefined Behavior";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		type = "Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		type = "Performance";
		break;
	case GL_DEBUG_TYPE_MARKER:
		type = "Marker";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		type = "Push Group";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		type = "Pop Group";
		break;
	case GL_DEBUG_TYPE_OTHER:
		type = "Other";
		break;
	}

	switch(severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH OpenGL: " << message << " SOURCE: " << source << " TYPE: " << type << std::endl;
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MED OpenGL: " << message << " SOURCE: " << source << " TYPE: " << type << std::endl;
		break;
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "LOW OpenGL: " << message << " SOURCE: " << source << " TYPE: " << type << std::endl;
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		break;
	}
}

void gl_check_leaked_handles() {

	#define GL_CHECK(type) if(glIs##type && glIs##type(i)) { std::cout << "Leaked OpenGL handle " << i << " of type " << #type << std::endl; leaked = true;}

	bool leaked = false;
	for(GLuint i = 0; i < 10000; i++) {
		GL_CHECK(Texture);
		GL_CHECK(Buffer);
		GL_CHECK(Framebuffer);
		GL_CHECK(Renderbuffer);
		GL_CHECK(VertexArray);
		GL_CHECK(Program);
		GL_CHECK(ProgramPipeline);
		GL_CHECK(Query);

		if(glIsShader(i)) {

			leaked = true;
			GLint shader_len = 0;
			glGetShaderiv(i, GL_SHADER_SOURCE_LENGTH, &shader_len);

			GLchar* shader = (GLchar*)malloc(shader_len);
			glGetShaderSource(i, shader_len, null, shader);

			std::string shader_str(shader);
			
			std::cout << "Leaked OpenGL shader " << i << ", source " << shader_str << std::endl; 

			free(shader);
		}
	}

	if(!leaked) {
		std::cout << "No OpenGL Objects Leaked!" << std::endl;
	}

	#undef GL_CHECK
}
