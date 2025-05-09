#version 330 core

layout (location = 0) in vec3 a_pos;

uniform mat4 u_Projection;
uniform mat4 u_View;

out vec3 texCoords;

void main()
{
	texCoords = a_pos;
	vec4 pos = u_Projection * u_View * vec4(a_pos, 1.0);
	gl_Position = pos.xyww;
}