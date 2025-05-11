#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vs_out {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
} i_gs[];

uniform mat4 u_Projection;
uniform float u_Time;

out gs_out {
	vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
} o_gs;



vec3 getNormal();
vec4 explode(vec4 vertexPos, vec3 normal);


void main()
{
	vec3 normal = getNormal();

	// Perform operation in view space because it is linear and easier to work with
	// When done, don't forget to multiply the results by the projection matrix

	gl_Position = u_Projection * explode(gl_in[0].gl_Position, normal);

	// Set fragment data for each vertex before emitting it so that the fragment shader gets the correct data
	o_gs.fragPos = i_gs[0].fragPos;
	o_gs.normal = i_gs[0].normal;
	o_gs.texCoords = i_gs[0].texCoords;
	EmitVertex();

	gl_Position = u_Projection * explode(gl_in[1].gl_Position, normal);
	o_gs.fragPos = i_gs[1].fragPos;
	o_gs.normal = i_gs[1].normal;
	o_gs.texCoords = i_gs[1].texCoords;
	EmitVertex();

	gl_Position = u_Projection * explode(gl_in[2].gl_Position, normal);
	o_gs.fragPos = i_gs[2].fragPos;
	o_gs.normal = i_gs[2].normal;
	o_gs.texCoords = i_gs[2].texCoords;
	EmitVertex();

	EndPrimitive();
}

vec3 getNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);

	return normalize(cross(a, b));
}

vec4 explode(vec4 vertexPos, vec3 normal)
{
	float magnitude = 1.0;
	vec3 offset = normal * ((sin(u_Time) + 1.0) / 2.0) * magnitude;
	return vertexPos + vec4(offset, 0.0);
}