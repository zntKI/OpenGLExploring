#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_texCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec2 texCoords;

void main()
{
	texCoords = a_texCoords;
	gl_Position = u_Projection * u_View * u_Model * vec4(a_pos, 1.0);
}