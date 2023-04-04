#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 tangent;	// GLSL thinks this is color
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 uv;

uniform mat4 mvp;
uniform mat3 model;
uniform mat3 normalMatrix;

out vec3 fragTangent;
out vec3 fragNormal;
out vec3 fragUV;

void main()
{
	gl_Position = mvp * vec4(position, 1.0);
	fragTangent = tangent;
	fragNormal = normal;
	fragUV = uv;
}