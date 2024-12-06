#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform vec3 u_objectColor;
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform vec3 u_viewPos;

out vec4 Color;

void main() {

   // Ambient
   float ambientStrength = 0.1;
   vec3 ambient = ambientStrength * u_lightColor;

   vec3 normal = normalize(Normal);
   vec3 lightDir = normalize(u_lightPos - FragPos);

   // Diffuse
   float diff = max(0.0, dot(lightDir, normal));
   vec3 diffuse = diff * u_lightColor;

   // Specular
   float specularStrength = 0.5;
   vec3 reflectedLightDir = reflect(-lightDir, normal);
   vec3 lookDir = normalize(u_viewPos - FragPos);
   float spec = pow(max(0.0, dot(lookDir, reflectedLightDir)), 64);
   vec3 specular = specularStrength * spec * u_lightColor;

   vec3 result = (ambient + diffuse + specular) * u_objectColor;
   Color = vec4(result, 1.0);

}