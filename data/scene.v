
#version 330 core

layout (location = 0) in vec3 v_pos;

// instanced
layout (location = 1) in vec4 m_color;
layout (location = 2) in vec4 m_pos;

uniform mat4 transform;

flat out uint f_id;
out vec4 f_color;

void main() {
	gl_Position = transform * vec4(v_pos + m_pos.xyz, 1.0);
	f_color = m_color;
	f_id = uint(m_pos.w);
}
