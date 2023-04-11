#version 410

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

uniform samplerCube envMap;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 ambientColor;

uniform vec3 spotLightColor;
uniform vec3 spotLightConeDir;
uniform vec3 spotLightPos;
uniform float spotLightCutoff;

in vec2 fragUV;
in mat3 TBN;
in vec3 objectPos;

out vec4 outColor;

void main()
{
	// Extracting color from texture and decoding gamma
	vec3 diffuseColor = pow(texture(diffuseTex, fragUV).xyz, vec3(2.2));

	// Normal mapping (never needs gamma correction)
	vec3 tsNormal = texture(normalTex, fragUV).xyz * 2 - 1;
	vec3 wsNormal = TBN * tsNormal;

	float nDotL = max(0.0, dot(wsNormal, lightDir));

	//Spotlight calculations
	vec3 toSpotLight = spotLightPos - objectPos;
	vec3 spotLightDir = normalize(toSpotLight);
	float cosAngle = dot(spotLightDir, -spotLightConeDir);
	float sFalloff = 1.0 / dot(toSpotLight, toSpotLight);
	vec3 normal = normalize(TBN[2]);
	vec3 spotLightIrr = sFalloff * spotLightColor * max(0, dot(normal, spotLightDir));

	vec3 envIrradiance = pow(textureLod(envMap, wsNormal, 99).xyz, vec3(2.2));

	vec3 irradiance = ambientColor + envIrradiance + lightColor * nDotL;
	
	if (cosAngle > spotLightCutoff) //only lit if inside cutoff
	{
		irradiance += spotLightIrr;
	}

	// endcode gamma
	outColor = vec4(pow(diffuseColor * irradiance, vec3(1.0/2.2)), 1.0);

}