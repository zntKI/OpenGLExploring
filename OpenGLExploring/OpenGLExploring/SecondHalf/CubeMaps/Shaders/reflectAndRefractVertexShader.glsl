#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat3 u_NormalMatrix;

out vec3 fragPos;
out vec3 normal;

void main()
{
    fragPos = vec3(u_Model * vec4(a_pos, 1.0));
    normal = u_NormalMatrix * a_normal;

    gl_Position = u_Projection * u_View * u_Model * vec4(a_pos, 1.0);
}