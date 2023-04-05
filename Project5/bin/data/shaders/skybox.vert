#version 410

layout (location = 0) in vec3 position;

uniform mat4 mvp;

// Untransformed local space position
out vec3 fragPos;

void main()
{
	fragPos = position;

	// Hack for putting the skybox at the far plane
	gl_Position = (mvp * vec4(position, 1.0)).xyww;
}