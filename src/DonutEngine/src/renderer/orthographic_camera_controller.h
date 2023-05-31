#ifndef ORTHOGRAPHIC_CAMERA_CONTROLLER_H
#define ORTHOGRAPHIC_CAMERA_CONTROLLER_H

#include "renderer/orthographic_camera.h"
#include "core/timestep.h"
#include "events/event.h"
#include "events/mouse_event.h"
#include "events/application_event.h"

namespace Donut
{
	struct OrthographicCameraBounds
	{
		float left_, right_;
		float bottom_, top_;

		float getWidth() { return right_ - left_; }
		float getHeight() { return top_ - bottom_; }
	};

	class OrthographicCameraController
	{
	public:

		OrthographicCameraController(float aspect_ratio, bool rotation = false);

		void onUpdate(Timestep ts);
		void onEvent(Event& ev);

		OrthographicCamera& getCamera() { return camera_; }
		const OrthographicCamera& getCamera() const { return camera_; }

		float getZoomLevel() const { return zoom_level_; }
		void setZoomLevel(float value) { zoom_level_ = value; }

		const OrthographicCameraBounds& getBounds() const { return ortho_cam_bound_; }

	private:
		bool onMouseScrolledEvent(MouseScrolledEvent& ev);
		bool onWindowResizedEvent(WindowResizeEvent& ev);

	private:
		float aspect_ratio_;
		float zoom_level_ = 1.0f;

		OrthographicCamera camera_;
		bool rotation_ = false;

		glm::vec3 camera_pos_ = { 0.0f, 0.0f, 0.0f };
		float camera_rotation_ = 0.0f;
		float rotation_speed_ = 30.0f;
		float camera_move_speed_ = 5.0f;

		OrthographicCameraBounds ortho_cam_bound_;
	};
}

#endif