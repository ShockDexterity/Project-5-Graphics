#version 410

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;
uniform sampler2D metallicTex;

uniform samplerCube envMap;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 ambientColor;

uniform vec3 spotLightColor;
uniform vec3 spotLightConeDir;
uniform vec3 spotLightPos;
uniform float spotLightCutoff;

uniform vec3 cameraPosition;

in vec2 fragUV;
in mat3 TBN;
in vec3 objectPos;

out vec4 outColor;

void main()
{
	// Extracting color from texture and decoding gamma
	vec3 diffuseColor = pow(texture(diffuseTex, fragUV).xyz, vec3(2.2));

	// Extracting specular color from texture and decoding gamma
	vec3 specularColor = mix(vec3(0.4), diffuseColor, texture(metallicTex, fragUV).xyz);
	specularColor = pow(specularColor, vec3(2.2));

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

	//irradiance - decoding gamma
	vec3 envIrradiance = pow(textureLod(envMap, wsNormal, 99).xyz, vec3(2.2));

	// how much light is effectively recieved by the surface
	vec3 irradiance = ambientColor + envIrradiance + lightColor * nDotL;

	//specular calculations
		//calculate view vector (direction from surface to camera)
	vec3 view = cameraPosition - objectPos;
	vec3 envLightDir = reflect(-view, wsNormal);

	//environment map reflection - encoding gamma
	vec3 envReflection = pow(texture(envMap, envLightDir).xyz, vec3(2.2));

	
	if (cosAngle > spotLightCutoff) //only lit if inside cutoff
	{
		irradiance += spotLightIrr;
	}

	vec3 ambientDiffuse = diffuseColor.rgb * irradiance;
	vec3 specularReflection = specularColor * envReflection;

	// endcode gamma
	outColor = vec4(pow(ambientDiffuse + specularReflection, vec3(1.0 / 2.2)), 1.0);

}