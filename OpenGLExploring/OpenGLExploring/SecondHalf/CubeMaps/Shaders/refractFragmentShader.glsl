#version 330 core

in vec3 fragPos;
in vec3 normal;

uniform samplerCube u_Skybox;
uniform vec3 u_ViewPos;

out vec4 o_fragColor;


void main()
{
	float ratio = 1.00 / 1.52;
	vec3 I = normalize(fragPos - u_ViewPos);
	vec3 R = refract(I, normalize(normal), ratio);
	o_fragColor = vec4(texture(u_Skybox, R).rgb, 1.0);

}