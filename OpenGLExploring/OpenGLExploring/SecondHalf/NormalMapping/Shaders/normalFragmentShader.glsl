#version 330 core

in VS_OUT {

	vec3 fragPos;
	vec2 texCoords;

} i_fs;

uniform sampler2D u_TexDiffuse;
uniform sampler2D u_TexNormal;

uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;

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
	vec3 lighting = ( ambient + diffuse + specular ) * color;

	o_fragColor = vec4(lighting, 1.0);
}