#version 330 core

in vec2 texCoords;

uniform sampler2D u_HdrBuffer;
uniform bool u_Hdr;
uniform float u_Exposure;


out vec4 o_fragColor;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(u_HdrBuffer, texCoords).rgb;
    if(u_Hdr)
    {
        // reinhard
        // vec3 result = hdrColor / (hdrColor + vec3(1.0));

        // exposure
        vec3 result = vec3(1.0) - exp(-hdrColor * u_Exposure);

        // also gamma correct while we're at it       
        result = pow(result, vec3(1.0 / gamma));
        o_fragColor = vec4(result, 1.0);
    }
    else
    {
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        o_fragColor = vec4(result, 1.0);
    }
}