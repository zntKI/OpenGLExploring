#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 u_ShadowMatrices[6];

out vec4 fragPos; // World pos passed down to fragment shader for each vertex

void main()
{
	for (int face = 0; face < 6; ++face)
	{
		gl_Layer = face; // built-in variable: to which face of the cubemap we render
		for (int i = 0; i < 3; ++i)
		{
			fragPos = gl_in[i].gl_Position;
			gl_Position = u_ShadowMatrices[face] * fragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}