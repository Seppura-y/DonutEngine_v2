#ifndef ORTHOGRAPHIC_CAMERA_H
#define ORTHOGRAPHIC_CAMERA_H

#include <glm/glm.hpp>

namespace Donut
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void setProjection(float left, float right, float bottom, float top);

		const glm::vec3& getPosition() const { return position_; }
		float getRotation() const { return rotation_; }

		void setPosition(const glm::vec3& position) { position_ = position; recalculateViewMatrix(); }
		void setRotation(float rotation) { rotation_ = rotation; recalculateViewMatrix(); }

		const glm::mat4& getProjectionMatrix() const { return projection_matrix_; }
		const glm::mat4& getViewMatrix() const { return view_matrix_; }
		const glm::mat4& getViewProjectionMatrix() const { return view_projection_matrix_; }
	private:
		void recalculateViewMatrix();
	private:
		glm::mat4 projection_matrix_;
		glm::mat4 view_matrix_;
		glm::mat4 view_projection_matrix_;

		glm::vec3 position_;
		float rotation_ = 0.0f;
	};
}

#endif