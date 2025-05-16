#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoords;
layout (location = 3) in mat4 a_instanceMatrix;

uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat3 u_NormalMatrix;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoords;

void main()
{
    fragPos = vec3(a_instanceMatrix * vec4(a_pos, 1.0));
    normal = u_NormalMatrix * a_normal;
    texCoords = a_texCoords;

    gl_Position = u_Projection * u_View * a_instanceMatrix * vec4(a_pos, 1.0);
}