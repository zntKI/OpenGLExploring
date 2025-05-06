#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

#define OFFSET_AMOUNT 0.05

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(a_pos + a_normal * OFFSET_AMOUNT, 1.0);
}