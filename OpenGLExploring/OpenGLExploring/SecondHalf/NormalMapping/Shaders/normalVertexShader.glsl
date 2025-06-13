#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoords;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec2 a_bitangent;

uniform mat4 u_Proj;
uniform mat4 u_View;
uniform mat4 u_Model;

uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;

out VS_OUT {

	vec3 fragPos;
	vec2 texCoords;
	vec3 tangentLightPos;
	vec3 tangentViewPos;
	vec3 tangentFragPos;

} o_vs;

void main()
{
	o_vs.fragPos = vec3(u_Model * vec4(a_pos, 1.0));
	o_vs.texCoords = a_texCoords;

	// Construct the TBN matrix
	vec3 T = normalize(vec3(u_Model * vec4(a_tangent, 0.0)));
	vec3 N = normalize(vec3(u_Model * vec4(a_normal, 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = mat3(T, B, N);

	// Transforming world pos vectors in tangent space
	// Working in tangent space because it is more efficient:
	//	- Able to do the costly operation in the vertex shader and send the result to the fragment shader
	//	- This way, we do way less calculations because vertex shaders are run much less than fragments
	o_vs.tangentLightPos = TBN * u_LightPos;
	o_vs.tangentViewPos = TBN * u_ViewPos;
	o_vs.tangentFragPos = TBN * o_vs.fragPos;

	gl_Position = u_Proj * u_View * u_Model * vec4(a_pos, 1.0);
}