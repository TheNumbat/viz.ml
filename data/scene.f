
#version 330 core

in vec4 f_color;
layout (location = 0) out vec3 color;
layout (location = 1) out vec3 id;

flat in uint f_id;

void main() {
	color = f_color.xyz;
	id 	  = vec3((f_id & 0xffu) / 255.0f, ((f_id & 0xff00u) >> 8u) / 255.0f, ((f_id & 0xff0000u) >> 16u) / 255.0f);
}

