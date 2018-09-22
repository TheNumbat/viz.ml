
#version 330 core

in vec4 f_color;
out vec4 color;

flat in uint id;
uniform bool use_id;

void main() {
	if(use_id) {
		color = vec4((id & 0xffu) / 255.0f, ((id & 0xff00u) >> 8u) / 255.0f, ((id & 0xff0000u) >> 16u) / 255.0f, 1.0f);
	} else {
		color = f_color;
	}
}
