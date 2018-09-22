
#version 330 core

in vec2 f_uv;

out vec4 color;

uniform sampler2DMS tex;
uniform int samples;
uniform vec2 tex_size;

void main() {

	ivec2 coord = ivec2(f_uv * tex_size);

	color = vec4(0.0);

	for (int i = 0; i < samples; i++)
		color += texelFetch(tex, coord, i);

	color /= float(samples);
}

