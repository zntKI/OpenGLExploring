#version 330 core

in vec2 texCoords;

uniform sampler2D u_Scene;
uniform sampler2D u_BloomBlur;
uniform bool u_Bloom;
uniform float u_Exposure;

out vec4 o_fragColor;

void main()
{
    const float gamma = 2.2;

    vec3 hdrColor = texture(u_Scene, texCoords).rgb;
    vec3 bloomColor = texture(u_BloomBlur, texCoords).rgb;

    if(u_Bloom)
        hdrColor += bloomColor; // additive blending

    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * u_Exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));

    o_fragColor = vec4(result, 1.0);
}