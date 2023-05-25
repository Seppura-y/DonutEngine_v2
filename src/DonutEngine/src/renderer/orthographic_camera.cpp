#include "pch.h"
#include "orthographic_camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Donut
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: projection_matrix_(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), view_matrix_(1.0f)
	{
		view_projection_matrix_ = projection_matrix_ * view_matrix_;
	}

	void OrthographicCamera::setProjection(float left, float right, float bottom, float top)
	{
		projection_matrix_ = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		view_projection_matrix_ = projection_matrix_ * view_matrix_;
	}

	void OrthographicCamera::recalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position_) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rotation_), glm::vec3(0, 0, 1));

		view_matrix_ = glm::inverse(transform);
		view_projection_matrix_ = projection_matrix_ * view_matrix_;
	}
}