#version 330 core

in vec3 fColor;

out vec4 o_fragColor;

void main()
{
	o_fragColor = vec4(fColor, 1.0);
}