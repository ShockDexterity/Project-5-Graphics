#pragma once
#include "ofMain.h"

class SpotLight
{
public:
	SpotLight() : color(1.0f), intensity(1.0f), cutoff(glm::cos(glm::radians(45.0f))), position(0.0f),
	              direction(0.0f, -1.0f, 0.0f) { }

	SpotLight(const glm::vec3& color, const float intensity, const float cutoff, const glm::vec3& position,
	          const glm::vec3& direction)
		: color(color), intensity(intensity), cutoff(cutoff), position(position),
		  direction(direction) { }

	glm::vec3 getColorIntensity() const
	{
		return color * intensity;
	}

	float getCutoff() const { return cutoff; }
	glm::vec3 getPosition() const { return position; }
	glm::vec3 getDirection() const { return direction; }

	void translate(const glm::vec3& translation) { this->position += translation; }

private:
	glm::vec3 color {};
	float intensity {};
	float cutoff {};

	glm::vec3 position {};
	glm::vec3 direction {};
};
