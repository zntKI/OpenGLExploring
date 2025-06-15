#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoords;


uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

uniform bool u_InverseNormals;


out VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
} o_vs;


void main()
{
    o_vs.fragPos = vec3(u_Model * vec4(a_pos, 1.0));   
    o_vs.texCoords = a_texCoords;
    
    vec3 n = u_InverseNormals ? -a_normal : a_normal;
    
    mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
    o_vs.normal = normalize(normalMatrix * n);
    
    gl_Position = u_Projection * u_View * u_Model * vec4(a_pos, 1.0);
}