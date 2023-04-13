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

uniform float roughness;

in vec2 fragUV;
in vec3 fragNormal;
in mat3 TBN;
in vec3 objectPos;

out vec4 outColor;

vec3 calcCookTorr(vec3 lightDirection, vec3 viewVec, vec3 worldNormal, vec3 specColor, float nDotL)
{
	//setup
	vec3 halfway = normalize(lightDirection + viewVec);
	float nDotH = max(0.0, dot(worldNormal, halfway));
	float nDotV = max(0.0, dot(worldNormal, viewVec));

	float hDotL = clamp(dot(halfway, lightDir),0.0, 1.0);
	vec3 fresnelHighlight = mix(specColor, vec3(1), pow(1 - hDotL, 5)); // F term
	float Ddenom = mix(1, roughness * roughness, nDotH * nDotH);
	Ddenom *= Ddenom;
	float D = (roughness * roughness) / Ddenom; // D term

	//G term
	float Gdenom = (nDotV * sqrt(mix(nDotL * nDotL, 1, roughness * roughness))) + (nDotL * sqrt(mix(nDotV * nDotV, 1, roughness * roughness)));
	float G = 0.5 / max(0.001, Gdenom);
	return D * fresnelHighlight * G;
}

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
	vec3 sNormal = normalize(fragNormal);
	vec3 spotLightIrr = sFalloff * spotLightColor * max(0, dot(sNormal, spotLightDir));

	float spotLightNDotL =  max(0.0, dot(wsNormal, spotLightDir));
	//DIFFUSE CALCULATIONS

	//irradiance - decoding gamma
	vec3 envIrradiance = pow(textureLod(envMap, wsNormal, 99).xyz, vec3(2.2));

	// how much light is effectively recieved by the surface
	vec3 irradiance = ambientColor + envIrradiance + lightColor * nDotL;

	if (cosAngle > spotLightCutoff) //only lit if inside cutoff
	{
		irradiance += spotLightIrr;
	}

	vec3 ambientDiffuse = diffuseColor.rgb * irradiance;

	//SPECULAR CALCULATIONS

	//calculate view vector (direction from surface to camera)
	vec3 view = normalize(cameraPosition - objectPos);
	vec3 envLightDir = reflect(-view, wsNormal);

	//environment map reflection - encoding gamma
	vec3 envReflection = pow(texture(envMap, envLightDir).xyz, vec3(2.2));

	//fresnel effect calculations
	float envNDotL = clamp(dot(wsNormal, envLightDir), 0.0, 1.0);
	vec3 fresnel = mix(specularColor, vec3(1), pow((1 - envNDotL), 5));

	//specular reflection calculation
	vec3 specularReflection = fresnel * envReflection;

	//DIRECTIONAL LIGHT COOKTORR
	//specular highlight calculation (Cook-Torrance, GGX, Smith)
//	vec3 halfway = normalize(lightDir + view);
//	float nDotH = max(0.0, dot(wsNormal, halfway));
//	float nDotV = max(0.0, dot(wsNormal, view));
//
//	float hDotL = clamp(dot(halfway, lightDir),0.0, 1.0);
//	vec3 fresnelHighlight = mix(specularColor, vec3(1), pow(1 - hDotL, 5)); // F term
//	float Ddenom = mix(1, roughness * roughness, nDotH * nDotH);
//	Ddenom *= Ddenom;
//	float D = (roughness * roughness) / Ddenom; // D term
//
//	//G term
//	float Gdenom = (nDotV * sqrt(mix(nDotL * nDotL, 1, roughness * roughness))) + (nDotL * sqrt(mix(nDotV * nDotV, 1, roughness * roughness)));
//	float G = 0.5 / max(0.001, Gdenom);

	vec3 cookTorr = calcCookTorr(lightDir, view, wsNormal, specularColor, nDotL);//D * fresnelHighlight * G;
	vec3 spotCookTorr = calcCookTorr(spotLightDir, view, wsNormal, specularColor, spotLightNDotL);

	//cookTorr *= nDotL * spotLightIrr;

	//Calculate highlight intensity

	vec3 totalColor = ambientDiffuse + specularReflection + cookTorr; 

	// endcode gamma
	outColor = vec4(pow(totalColor, vec3(1.0 / 2.2)), 1.0);

}