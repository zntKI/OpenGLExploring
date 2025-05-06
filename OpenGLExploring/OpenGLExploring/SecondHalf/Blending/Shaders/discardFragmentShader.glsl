#version 330 core

in vec2 texCoords;

uniform sampler2D u_Texture;

out vec4 o_fragColor;

void main()
{
	vec4 texColor = texture(u_Texture, texCoords);
	if (texColor.a < 0.1)
		discard;

	o_fragColor = texColor;
}