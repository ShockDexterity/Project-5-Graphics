#pragma once
#include "ofMain.h"

struct SpotLight
{
public:
	glm::vec3 position;
	glm::vec3 color{ 1 };
	float intensity{ 1 };
	glm::vec3 direction;
	float cutoff;

	glm::vec3 getColorIntensity() const
	{
		return color * intensity;
	}
};