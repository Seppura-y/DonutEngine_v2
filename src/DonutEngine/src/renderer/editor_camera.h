#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include "camera.h"
#include "core/timestep.h"
#include "events/event.h"
#include "events/mouse_event.h"

#include <glm/glm.hpp>

namespace Donut
{
	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspect_ratio, float near_clip, float far_clip);

		void onUpdate(Timestep ts);
		void onEvent(Event& e);

		inline float getDistance() const { return distance_; }
		inline void setDistance(float distance) { distance_ = distance; }

		inline void setViewportSize(float width, float height) { viewport_width_ = width; viewport_height_ = height; updateProjection(); }

		const glm::mat4& getViewMatrix() const { return view_matrix_; }
		glm::mat4 getViewProjection() const { return projection_ * view_matrix_; }

		glm::vec3 getUpDirection() const;
		glm::vec3 getRightDirection() const;
		glm::vec3 getForwardDirection() const;
		const glm::vec3& getPosition() const { return position_; }
		glm::quat getOrientation() const;

		float getPitch() const { return pitch_; }
		float getYaw() const { return yaw_; }
	private:
		void updateProjection();
		void updateView();

		bool onMouseScroll(MouseScrolledEvent& e);

		void mousePan(const glm::vec2& delta);
		void mouseRotate(const glm::vec2& delta);
		void mouseZoom(float delta);

		glm::vec3 calculatePosition() const;

		std::pair<float, float> panSpeed() const;
		float rotationSpeed() const;
		float zoomSpeed() const;
	private:
		float fov_ = 45.0f, aspect_ratio_ = 1.778f, near_clip_ = 0.1f, far_clip_ = 1000.0f;

		glm::mat4 view_matrix_;
		glm::vec3 position_ = { 0.0f, 0.0f, 0.0f };
		glm::vec3 focal_point_ = { 0.0f, 0.0f, 0.0f };

		glm::vec2 initial_mouse_position_ = { 0.0f, 0.0f };

		float distance_ = 10.0f;
		float pitch_ = 0.0f, yaw_ = 0.0f;

		float viewport_width_ = 1280, viewport_height_ = 720;
	};
}

#endif