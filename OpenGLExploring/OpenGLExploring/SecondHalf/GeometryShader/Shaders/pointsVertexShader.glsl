#version 330 core

layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec3 a_color;

out vs_out {
	vec3 color;
} o_vs;

void main()
{
	o_vs.color = a_color;
	gl_Position = vec4(a_pos, 0.0, 1.0);
}