#version 330 core

in vec2 texCoords;

uniform sampler2D u_Image;

uniform bool u_Horizontal;
uniform float u_Weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);


out vec4 o_fragColor;

void main()
{             
     vec2 tex_offset = 1.0 / textureSize(u_Image, 0); // gets size of single texel
     vec3 result = texture(u_Image, texCoords).rgb * u_Weight[0];

     if(u_Horizontal)
     {
         for(int i = 1; i < 5; ++i)
         {
            result += texture(u_Image, texCoords + vec2(tex_offset.x * i, 0.0)).rgb * u_Weight[i];
            result += texture(u_Image, texCoords - vec2(tex_offset.x * i, 0.0)).rgb * u_Weight[i];
         }
     }
     else
     {
         for(int i = 1; i < 5; ++i)
         {
             result += texture(u_Image, texCoords + vec2(0.0, tex_offset.y * i)).rgb * u_Weight[i];
             result += texture(u_Image, texCoords - vec2(0.0, tex_offset.y * i)).rgb * u_Weight[i];
         }
     }

     o_fragColor = vec4(result, 1.0);
}