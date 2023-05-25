#ifndef ORTHOGRAPHIC_CAMERA_CONTROLLER_H
#define ORTHOGRAPHIC_CAMERA_CONTROLLER_H

#include "renderer/orthographic_camera.h"
#include "core/timestep.h"
#include "events/event.h"
#include "events/mouse_event.h"
#include "events/application_event.h"

namespace Donut
{
	class OrthographicCameraController
	{
	public:

		OrthographicCameraController(float aspect_ratio, bool rotation = false);

		void onUpdate(Timestep ts);
		void onEvent(Event& ev);

		OrthographicCamera& getCamera() { return camera_; }
		const OrthographicCamera& getCamera() const { return camera_; }

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
	};
}

#endif