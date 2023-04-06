#include "CameraMatrices.h"

using namespace glm;

CameraMatrices::CameraMatrices(const Camera& camera, const float aspect, const float nearPlane, const float farPlane)
	: camera { camera },
	  view { mat4(transpose(camera.rotation)) * glm::translate(-camera.position) },
	  proj { perspective(camera.fov, aspect, nearPlane, farPlane) } { }

const Camera& CameraMatrices::getCamera() const
{
	return camera;
}

const glm::mat4& CameraMatrices::getView() const
{
	return view;
}

const glm::mat4& CameraMatrices::getProj() const
{
	return proj;
}
