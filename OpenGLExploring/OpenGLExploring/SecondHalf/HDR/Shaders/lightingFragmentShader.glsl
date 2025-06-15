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
uniform Light u_Lights[16];

uniform sampler2D u_DiffuseTexture;

uniform vec3 u_ViewPos;


out vec4 o_fragColor;


void main()
{
    vec3 color = texture(u_DiffuseTexture, i_fs.texCoords).rgb;
    vec3 normal = normalize(i_fs.normal);

    // ambient
    vec3 ambient = 0.0 * color;
    // lighting
    vec3 lighting = vec3(0.0);

    for(int i = 0; i < 16; i++)
    {
        // diffuse
        vec3 lightDir = normalize(u_Lights[i].Position - i_fs.fragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = u_Lights[i].Color * diff * color;
        vec3 result = diffuse;

        // attenuation (use quadratic as we have gamma correction)
        float distance = length(i_fs.fragPos - u_Lights[i].Position);
        result *= 1.0 / (distance * distance);
        lighting += result;
    }

    o_fragColor = vec4(ambient + lighting, 1.0);
}