#include "pch.h"
#include "orthographic_camera_controller.h"

#include "core/input.h"
#include "key_codes.h"
#include "mouse_codes.h"

namespace Donut
{
	OrthographicCameraController::OrthographicCameraController(float aspect_ratio, bool rotation)
		: aspect_ratio_(aspect_ratio), camera_(-aspect_ratio_ * zoom_level_, aspect_ratio_ * zoom_level_, -zoom_level_, zoom_level_), rotation_(rotation)
	{

	}

	void OrthographicCameraController::onUpdate(Timestep ts)
	{
		DN_PROFILE_FUNCTION();

		if (Input::isKeyPressed(DN_KEY_A))
		{
			camera_pos_.x += camera_move_speed_ * ts;
		}

		if (Input::isKeyPressed(DN_KEY_D))
		{
			camera_pos_.x -= camera_move_speed_ * ts;
		}

		if (Input::isKeyPressed(DN_KEY_W))
		{
			camera_pos_.y -= camera_move_speed_ * ts;
		}

		if (Input::isKeyPressed(DN_KEY_S))
		{
			camera_pos_.y += camera_move_speed_ * ts;
		}

		if (rotation_)
		{
			if (Input::isKeyPressed(DN_KEY_Q))
			{
				camera_rotation_ -= rotation_speed_ * ts;
			}

			if (Input::isKeyPressed(DN_KEY_E))
			{
				camera_rotation_ += rotation_speed_ * ts;
			}

			camera_.setRotation(camera_rotation_);
		}

		camera_.setPosition(camera_pos_);

		// the higher the zoom_level_, the faster the camera moves
		camera_move_speed_ = zoom_level_;
	}

	void OrthographicCameraController::onEvent(Event& ev)
	{
		DN_PROFILE_FUNCTION();

		EventDispatcher dispatcher(ev);
		dispatcher.dispatch<MouseScrolledEvent>(DN_BIND_EVENT_FN(OrthographicCameraController::onMouseScrolledEvent));
		dispatcher.dispatch<WindowResizeEvent>(DN_BIND_EVENT_FN(OrthographicCameraController::onWindowResizedEvent));
	}

	bool OrthographicCameraController::onMouseScrolledEvent(MouseScrolledEvent& ev)
	{
		DN_PROFILE_FUNCTION();

		zoom_level_ -= ev.getYOffset() * 0.4f;
		zoom_level_ = std::max(zoom_level_, 0.25f);
		camera_.setProjection(-aspect_ratio_ * zoom_level_, aspect_ratio_ * zoom_level_, -zoom_level_, zoom_level_);
		return false;
	}

	bool OrthographicCameraController::onWindowResizedEvent(WindowResizeEvent& ev)
	{
		DN_PROFILE_FUNCTION();

		aspect_ratio_ = (float)ev.getWidth() / (float)ev.getHeight();
		camera_.setProjection(-aspect_ratio_ * zoom_level_, aspect_ratio_ * zoom_level_, -zoom_level_, zoom_level_);
		return false;
	}
}