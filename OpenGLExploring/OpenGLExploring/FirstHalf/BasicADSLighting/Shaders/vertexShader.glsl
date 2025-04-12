#version 330 core

out vec3 FragPos;
out vec3 Normal;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
	
void main(){
   FragPos = vec3(u_model * vec4(i_position, 1.0));
   // Inversing matrices is a costly operation for shaders, so wherever possible try to avoid
   // doing inverse operations since they have to be done on each vertex of your scene. For
   // learning purposes this is fine, but for an efficient application you’ll likely want to calculate
   // the normal matrix on the CPU and send it to the shaders via a uniform before drawing
   // (just like the model matrix).
   Normal = mat3(transpose(inverse(u_model))) * i_normal;

   gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0);
}