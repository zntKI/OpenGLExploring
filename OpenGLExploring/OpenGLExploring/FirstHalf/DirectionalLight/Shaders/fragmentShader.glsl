#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 u_viewPos;

struct Material {
   sampler2D diffuse;
   sampler2D specular;
   sampler2D emission;

   float shininess;
};
uniform Material material;
struct Light {
   vec4 direction;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};
uniform Light light;

out vec4 Color;

void main() {

   // Ambient
   vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

   vec3 normal = normalize(Normal);
   vec3 lightDir = normalize(-light.direction.xyz);

   // Diffuse
   float diff = max(0.0, dot(lightDir, normal));
   vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));

   // Specular
   vec3 reflectedLightDir = reflect(-lightDir, normal);
   vec3 lookDir = normalize(u_viewPos - FragPos);
   float spec = pow(max(0.0, dot(lookDir, reflectedLightDir)), material.shininess);
   vec3  texValue = vec3(texture(material.specular, TexCoords));
   vec3 specular = light.specular * (spec * texValue);

   // Emission
   vec3 emission = texValue * vec3(texture(material.emission, TexCoords));

   vec3 result = ambient + diffuse + specular + emission;
   Color = vec4(result, 1.0);

}