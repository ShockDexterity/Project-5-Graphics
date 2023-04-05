#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 tangent;	// GLSL thinks this is color
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

uniform mat4 mvp;
uniform mat4 model;
uniform mat3 normalMatrix;

out vec4 fragTangent;
out vec3 fragNormal;
out vec2 fragUV;

void main()
{
	gl_Position = mvp * vec4(position, 1.0);
	fragTangent = tangent;
	fragNormal = normal;
	fragUV = uv;
}