#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 color;

out vec3 ourColor;
	
void main(){
   gl_Position = position;

   ourColor = gl_Position.xyz;
}