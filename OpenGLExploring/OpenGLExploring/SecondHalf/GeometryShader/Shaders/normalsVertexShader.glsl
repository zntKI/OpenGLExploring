#version 330 core

layout (location = 0) in vec3 a_pos;

uniform mat4 u_Model;
uniform mat4 u_View;

void main()
{
	// Perform operation in view space because it is linear and easier to work with
	// Don't forget to multiply the results by the projection matrix in the geometry shader

	gl_Position = u_View * u_Model * vec4(a_pos, 1.0);
}