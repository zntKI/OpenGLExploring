#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform vec3 u_viewPos;

struct Material {
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   float shininess;
};
uniform Material material;
struct Light {
   vec3 position;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};
uniform Light light;

out vec4 Color;

void main() {

   // Ambient
   vec3 ambient = light.ambient * material.ambient;

   vec3 normal = normalize(Normal);
   vec3 lightDir = normalize(light.position - FragPos);

   // Diffuse
   float diff = max(0.0, dot(lightDir, normal));
   vec3 diffuse = light.diffuse * (diff * material.diffuse);

   // Specular
   vec3 reflectedLightDir = reflect(-lightDir, normal);
   vec3 lookDir = normalize(u_viewPos - FragPos);
   float spec = pow(max(0.0, dot(lookDir, reflectedLightDir)), material.shininess);
   vec3 specular = light.specular * (spec * material.specular);

   vec3 result = ambient + diffuse + specular;
   Color = vec4(result, 1.0);

}