#version 330 core

in vec2 texCoords;

uniform sampler2D u_Texture;

out vec4 o_fragColor;

void main()
{
	o_fragColor = texture(u_Texture, texCoords);
}