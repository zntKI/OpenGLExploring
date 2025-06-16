#version 330 core

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
} i_fs;

uniform vec3 u_LightColor;

layout (location = 0) out vec4 o_fragColor;
layout (location = 1) out vec4 o_brightColor;

void main()
{
    o_fragColor = vec4(u_LightColor, 1.0);

    float brightness = dot(o_fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        o_brightColor = vec4(o_fragColor.rgb, 1.0);
	else
		o_brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}