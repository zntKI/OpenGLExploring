#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;

uniform mat4 transform;

out vec3 ourColor;
out vec2 TexCoord;
	
void main(){
   gl_Position = transform * vec4(position.xyz, 1.0f);

   ourColor = color;
   TexCoord = texCoord;
}