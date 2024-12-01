#version 330 core

layout(location = 0) in vec4 position;
layout(location = 2) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
	
void main(){
   gl_Position = projection * view * model * vec4(position.xyz, 1.0);

   TexCoord = texCoord;
}