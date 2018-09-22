
#include "gl.h"
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

void mesh::init() {

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &v_vbo);
	glGenBuffers(1, &c_vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, v_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, c_vbo);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBindVertexArray(0);
}

void mesh::destroy() {

	glDeleteBuffers(1, &v_vbo);
	glDeleteBuffers(1, &c_vbo);
	glDeleteBuffers(1, &ebo);	
	glDeleteVertexArrays(1, &vao);

	v_vbo = c_vbo = ebo = vao = 0;
}

void mesh::clear() {
	vertices.clear();
	colors.clear();
	elements.clear();
	dirty = true;
}

void mesh::update() {

	if(!dirty) return;

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, v_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v3) * vertices.size(), vertices.size() ? &vertices[0] : null, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, c_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorf) * colors.size(), colors.size() ? &colors[0] : null, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uv3) * elements.size(), elements.size() ? &elements[0] : null, GL_STATIC_DRAW);

	glBindVertexArray(0);

	dirty = false;
}

void mesh::render() {

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, (GLsizei)elements.size() * 3, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
}

void mesh::push_cube(v3 pos, f32 len) {

	i32 idx = (i32)vertices.size();

	f32 len2 = len / 2.0f;
	vertices.push_back(pos + v3( len2,  len2,  len2));
	vertices.push_back(pos + v3(-len2,  len2,  len2));
	vertices.push_back(pos + v3( len2, -len2,  len2));
	vertices.push_back(pos + v3( len2,  len2, -len2));
	vertices.push_back(pos + v3(-len2, -len2,  len2));
	vertices.push_back(pos + v3( len2, -len2, -len2));
	vertices.push_back(pos + v3(-len2,  len2, -len2));
	vertices.push_back(pos + v3(-len2, -len2, -len2));

	colors.push_back(colorf(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(colorf(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(colorf(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(colorf(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(colorf(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(colorf(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(colorf(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(colorf(1.0f, 0.0f, 0.0f, 1.0f));	

	elements.push_back(uv3(idx + 0, idx + 3, idx + 6));
	elements.push_back(uv3(idx + 0, idx + 3, idx + 5));
	elements.push_back(uv3(idx + 0, idx + 1, idx + 6));
	elements.push_back(uv3(idx + 1, idx + 4, idx + 7));
	elements.push_back(uv3(idx + 1, idx + 6, idx + 7));
	elements.push_back(uv3(idx + 4, idx + 2, idx + 5));
	elements.push_back(uv3(idx + 4, idx + 7, idx + 5));
	elements.push_back(uv3(idx + 7, idx + 5, idx + 3));
	elements.push_back(uv3(idx + 7, idx + 6, idx + 3));
	elements.push_back(uv3(idx + 0, idx + 2, idx + 4));
	elements.push_back(uv3(idx + 0, idx + 1, idx + 4));

	dirty = true;
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
