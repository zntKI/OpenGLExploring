#version 330 core

struct Light {
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};
uniform Light light;

out vec4 o_color;

void main() {
   o_color = vec4(light.ambient + light.diffuse, 1.0);
}