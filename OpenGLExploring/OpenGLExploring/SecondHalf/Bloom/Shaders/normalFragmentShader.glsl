#version 330 core

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
} i_fs;


struct Light {
    vec3 Position;
    vec3 Color;
};
uniform Light u_Lights[4];

uniform sampler2D u_DiffuseTexture;

uniform vec3 u_ViewPos;


layout (location = 0) out vec4 o_fragColor;
layout (location = 1) out vec4 o_brightColor;

void main()
{           
    vec3 color = texture(u_DiffuseTexture, i_fs.texCoords).rgb;
    vec3 normal = normalize(i_fs.normal);

    // ambient
    vec3 ambient = 0.0 * color;

    // lighting
    vec3 lighting = vec3(0.0);
    vec3 viewDir = normalize(u_ViewPos - i_fs.fragPos);
    for(int i = 0; i < 4; i++)
    {
        // diffuse
        vec3 lightDir = normalize(u_Lights[i].Position - i_fs.fragPos);
        float diff = max(dot(lightDir, normal), 0.0);

        vec3 result = u_Lights[i].Color * diff * color;

        // attenuation (use quadratic as we have gamma correction)
        float distance = length(i_fs.fragPos - u_Lights[i].Position);
        result *= 1.0 / (distance * distance);
        lighting += result;   
    }

    vec3 result = ambient + lighting;
    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));

    if(brightness > 1.0)
        o_brightColor = vec4(result, 1.0);
    else
        o_brightColor = vec4(0.0, 0.0, 0.0, 1.0);

    o_fragColor = vec4(result, 1.0);
}