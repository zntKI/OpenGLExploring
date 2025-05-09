#version 330 core

in vec3 texCoords;

uniform samplerCube u_Skybox;

out vec4 o_fragColor;

void main()
{
	o_fragColor = texture(u_Skybox, texCoords);
	//o_fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}