#version 330 core

in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

uniform vec3 u_ViewPos;

struct Material {
   sampler2D texture_diffuse1;
   sampler2D texture_specular1;

   float shininess;
};
uniform Material u_Material;

// Lights

struct DirectionalLight {
   vec3 direction;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};
uniform DirectionalLight u_DirectionalLight;

out vec4 o_fragColor;


vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 lookDir);

void main() {

   vec3 normal = normalize(normal);
   vec3 lookDir = normalize(u_ViewPos - fragPos);

   vec3 finalColor = vec3(0.0);

   // DirectionalLight
   finalColor = calculateDirectionalLight(u_DirectionalLight, normal, lookDir);

   o_fragColor = vec4(finalColor, 1.0);

}

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 lookDir)
{
   // Ambient
   vec3 ambient = light.ambient * vec3(texture(u_Material.texture_diffuse1, texCoords));

   // Diffuse
   vec3 lightDir = normalize(-light.direction);
   float diff = max(0.0, dot(lightDir, normal));
   vec3 diffuse = light.diffuse * (diff * vec3(texture(u_Material.texture_diffuse1, texCoords)));

   // Specular
   vec3 reflectedLightDir = reflect(-lightDir, normal);
   float spec = pow(max(0.0, dot(lookDir, reflectedLightDir)), u_Material.shininess);
   vec3  texValue = vec3(texture(u_Material.texture_specular1, texCoords));
   vec3 specular = light.specular * (spec * texValue);

   return (ambient + diffuse + specular);
}