#version 410

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 ambientColor;

in vec2 fragUV;
in mat3 TBN;

out vec4 outColor;

void main()
{
	// Extracting color from texture and decoding gamma
	vec3 diffuseColor = pow(texture(diffuseTex, fragUV).xyz, vec3(2.2));

	// Normal mapping (never needs gamma correction)
	vec3 tsNormal = texture(normalTex, fragUV).xyz * 2 - 1;
	vec3 wsNormal = TBN * tsNormal;


	float nDotL = max(0.0, dot(wsNormal, lightDir));
	vec3 irradiance = ambientColor + lightColor * nDotL;

	// endcode gamma
	outColor = vec4(pow(diffuseColor * irradiance, vec3(1.0/2.2)), 1.0);
}