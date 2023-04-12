#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 tangent;	// GLSL thinks this is color
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

uniform mat4 mvp;
uniform mat3 normalMatrix;
uniform mat4 model;

out vec2 fragUV;
out vec3 fragNormal;
out mat3 TBN;
out vec3 objectPos;

void main()
{
	gl_Position = mvp * vec4(position, 1.0);

	fragUV = vec2(uv.x, 1 - uv.y).xy;
	
	vec3 T = normalize(normalMatrix * tangent.xyz);
	vec3 B = normalize(normalMatrix * cross(tangent.xyz, normal));
	vec3 N = normalize(normalMatrix * normal);

	fragNormal = N;

	objectPos = (model * vec4(position, 1.0)).xyz;

	TBN = mat3(T, B, N);
}