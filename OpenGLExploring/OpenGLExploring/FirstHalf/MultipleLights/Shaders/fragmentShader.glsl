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

// Lights

struct DirectionalLight {
   vec4 direction;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};

struct PointLight {
   vec3 position;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   float constant;
   float linear;
   float quadratic;
};

struct SpotLight {
   vec3 position;
   vec3 direction;
   float cutOffInner;
   float cutOffOuter;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   float constant;
   float linear;
   float quadratic;
};

uniform DirectionalLight directionalLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

out vec4 Color;

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 lookDir, vec3 emission);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 lookDir, vec3 emission);
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 lookDir, vec3 emission);

void main() {

   vec3 normal = normalize(Normal);
   vec3 lookDir = normalize(u_viewPos - FragPos);

   // Emission (Part of)
   vec3 emission = vec3(texture(material.emission, TexCoords));

   vec3 finalColor = vec3(0.0);

   // DirectionalLight
   finalColor = calculateDirectionalLight(directionalLight, normal, lookDir, emission);

   // PointLightS
   for (int i = 0; i < NR_POINT_LIGHTS; i++)
      finalColor += calculatePointLight(pointLights[i], normal, lookDir, emission);

   // SpotLight
   finalColor += calculateSpotLight(spotLight, normal, lookDir, emission);

   Color = vec4(finalColor, 1.0);

}

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 lookDir, vec3 emission)
{
   // Ambient
   vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

   // Diffuse
   vec3 lightDir = normalize(-light.direction.xyz);
   float diff = max(0.0, dot(lightDir, normal));
   vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));

   // Specular
   //vec3 reflectedLightDir = reflect(-lightDir, normal);
   vec3 halfwayDir = normalize(lightDir + lookDir);
   float spec = pow(max(0.0, dot(halfwayDir, normal)), material.shininess);
   vec3  texValue = vec3(texture(material.specular, TexCoords));
   vec3 specular = light.specular * (spec * texValue);

   // Emission (modify to appear only on edges)
   emission *= texValue;

   return (ambient + diffuse + specular + emission);
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 lookDir, vec3 emission)
{
   // Ambient
   vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

   // Diffuse
   vec3 lightDir = normalize(light.position - FragPos);
   float diff = max(0.0, dot(lightDir, normal));
   vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));

   // Specular
   //vec3 reflectedLightDir = reflect(-lightDir, normal);
   vec3 halfwayDir = normalize(lightDir + lookDir);
   float spec = pow(max(0.0, dot(halfwayDir, normal)), material.shininess);
   vec3  texValue = vec3(texture(material.specular, TexCoords));
   vec3 specular = light.specular * (spec * texValue);

   // Emission (modify to appear only on edges)
   emission *= texValue;

   // Attenuation
   float distance = length(light.position - FragPos);
   float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

   ambient *= attenuation;
   diffuse *= attenuation;
   specular *= attenuation;

   return (ambient + diffuse + specular + emission);
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 lookDir, vec3 emission)
{
   // Ambient
   vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

   // Diffuse
   vec3 lightDir = normalize(light.position - FragPos);
   float diff = max(0.0, dot(lightDir, normal));
   vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));

   // Specular
   //vec3 reflectedLightDir = reflect(-lightDir, normal);
   vec3 halfwayDir = normalize(lightDir + lookDir);
   float spec = pow(max(0.0, dot(halfwayDir, normal)), material.shininess);
   vec3  texValue = vec3(texture(material.specular, TexCoords));
   vec3 specular = light.specular * (spec * texValue);

   // Emission (modify to appear only on edges)
   emission *= texValue;

   // Attenuation
   float distance = length(light.position - FragPos);
   float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

   // Light calculations
   float theta = dot(lightDir, normalize(-light.direction));
   float epsilon = light.cutOffInner - light.cutOffOuter;
   float intensity = clamp((theta - light.cutOffOuter) / epsilon, 0.0, 1.0);

   ambient *= attenuation;
   diffuse *= attenuation * intensity;
   specular *= attenuation * intensity;

   return (ambient + diffuse + specular + emission);
}