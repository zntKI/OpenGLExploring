#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat3 u_NormalMatrix;

out vs_out {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
} o_vs;

void main()
{
    o_vs.fragPos = vec3(u_Model * vec4(a_pos, 1.0));
    o_vs.normal = u_NormalMatrix * a_normal;
    o_vs.texCoords = a_texCoords;

    // Perform operation in view space because it is linear and easier to work with
	// Don't forget to multiply the results by the projection matrix in the geometry shader

    gl_Position = u_View * u_Model * vec4(a_pos, 1.0);
}