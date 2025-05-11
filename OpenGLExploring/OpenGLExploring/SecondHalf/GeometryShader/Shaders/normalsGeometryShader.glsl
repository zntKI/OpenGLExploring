#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

uniform mat4 u_Projection;
uniform float u_Time;

const float MAGNITUDE = 0.1;


void generateLine(int index);

vec4 getTriangleCentrePoint();
vec3 getNormal();
vec4 explode(vec4 vertexPos, vec3 normal);

void main()
{
	vec4 centerPoint = getTriangleCentrePoint(); // visualize the normal from the center of the triangle
	vec3 normal = getNormal();
	vec4 explodedPosition = explode(centerPoint, normal);

	// Perform operation in view space because it is linear and easier to work with
	// When done, don't forget to multiply the results by the projection matrix

	gl_Position = u_Projection * explodedPosition;
	EmitVertex();

	gl_Position = u_Projection * (explodedPosition + vec4(normal, 0.0) * MAGNITUDE);
	EmitVertex();

	EndPrimitive();
}

vec4 getTriangleCentrePoint()
{
	return vec4((vec3(gl_in[0].gl_Position) + vec3(gl_in[1].gl_Position) + vec3(gl_in[2].gl_Position))/3.0, 1.0);
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