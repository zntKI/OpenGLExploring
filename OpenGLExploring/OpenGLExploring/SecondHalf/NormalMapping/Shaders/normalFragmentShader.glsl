#version 330 core

in VS_OUT {

	vec3 fragPos;
	vec2 texCoords;
	vec3 tangentLightPos;
	vec3 tangentViewPos;
	vec3 tangentFragPos;

} i_fs;

uniform sampler2D u_TexDiffuse;
uniform sampler2D u_TexNormal;

out vec4 o_fragColor;

void main()
{
	// retrieve normal from normal map
	vec3 normal = texture(u_TexNormal, i_fs.texCoords).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	vec3 color = texture(u_TexDiffuse, i_fs.texCoords).rgb;
	vec3 lightColor = vec3(1.0);

	// ambient
	vec3 ambient = 0.15 * color;

	//diffuse
	vec3 lightDir = normalize(i_fs.tangentLightPos - i_fs.tangentFragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor;

	//specular
	vec3 viewDir = normalize(i_fs.tangentViewPos - i_fs.tangentFragPos);
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	vec3 specular = lightColor * spec;

	// Calculate shadow
	vec3 lighting = ( ambient + diffuse + specular ) * color;

	o_fragColor = vec4(lighting, 1.0);
}