#version 330 core

in vec3 fragPos;
in vec3 normal;

uniform samplerCube u_Skybox;
uniform vec3 u_ViewPos;

out vec4 o_fragColor;


void main() {

   vec3 I = normalize(fragPos - u_ViewPos);
   vec3 R = reflect(I, normalize(normal));
   o_fragColor = vec4(texture(u_Skybox, R).rgb, 1.0);

}