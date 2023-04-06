#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 tangent;	// GLSL thinks this is color
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

uniform mat4 mvp;
uniform mat3 normalMatrix;

out vec2 fragUV;
out mat3 TBN;

void main()
{
	gl_Position = mvp * vec4(position, 1.0);

	fragUV = vec2(uv.x, 1 - uv.y).xy;
	
	vec3 T = normalize(normalMatrix * tangent.xyz);
	vec3 B = normalize(normalMatrix * cross(tangent.xyz, normal));
	vec3 N = normalize(normalMatrix * normal);

	TBN = mat3(T, B, N);
}