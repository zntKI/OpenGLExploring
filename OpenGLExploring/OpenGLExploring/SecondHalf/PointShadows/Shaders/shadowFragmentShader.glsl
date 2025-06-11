#version 330 core

in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
} i_fs;

uniform sampler2D u_TexDiffuse;

uniform samplerCube u_ShadowMap;

uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;

uniform float u_FarPlane;

out vec4 o_fragColor;

float ShadowCalculation(vec3 fragmentNormal, vec3 lightDir);

void main()
{
	vec3 color = texture(u_TexDiffuse, i_fs.texCoords).rgb;
	vec3 normal = normalize(i_fs.normal);
	vec3 lightColor = vec3(1.0);

	// ambient
	vec3 ambient = 0.15 * color;

	//diffuse
	vec3 lightDir = normalize(u_LightPos - i_fs.fragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor;

	//specular
	vec3 viewDir = normalize(u_ViewPos - i_fs.fragPos);
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	vec3 specular = lightColor * spec;

	// Calculate shadow
	float shadowAmount = ShadowCalculation(normal, lightDir);
	vec3 lighting = ( ambient + (1.0 - shadowAmount) * (diffuse + specular) ) * color;

	o_fragColor = vec4(lighting, 1.0);
}

float ShadowCalculation(vec3 fragmentNormal, vec3 lightDir)
{
	vec3 fragPos = i_fs.fragPos;

	vec3 fragToLight = fragPos - u_LightPos;

	float currentDepth = length(fragToLight);

	// Simple way to reduce shadow acne
	// float bias = 0.005;
	// Better way adapting the value based on the angle of the light source
	float bias = max(0.5 * (1.0 - dot(fragmentNormal, lightDir)), 0.15);

	// PCF:

	vec3 sampleOffsetDirections[20] = vec3[]
	(
		vec3( 1, 1, 1), vec3( 1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
		vec3( 1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
		vec3( 1, 1, 0), vec3( 1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
		vec3( 1, 0, 1), vec3(-1, 0, 1), vec3( 1, 0, -1), vec3(-1, 0, -1),
		vec3( 0, 1, 1), vec3( 0, -1, 1), vec3( 0, -1, -1), vec3( 0, 1, -1)
	);

	float shadowAmount = 0.0;
	int samples = 20;
	float viewDistance = length(u_ViewPos - fragPos);
	float diskRadius = (1.0 + (viewDistance / u_FarPlane)) / 25.0;
	for (int i = 0; i < samples; ++i)
	{
		float closestPcfDepth = texture(u_ShadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestPcfDepth *= u_FarPlane; // Convert depth from [0,1] to [0,far_plane] range

		shadowAmount += currentDepth - bias > closestPcfDepth ? 1.0 : 0.0;
	}
	shadowAmount /= float(samples);

	return shadowAmount;
}