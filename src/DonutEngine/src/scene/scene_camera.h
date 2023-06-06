#ifndef SCENE_CAMERA_H
#define SCENE_CAMERA_H

#include "renderer/camera.h"

namespace Donut
{
	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		~SceneCamera() = default;

		void setOrthographic(float size, float near_clip, float far_clip);
		void setViewportSize(uint32_t width, uint32_t height);

		void setOrthographicSize(float size) { orthographic_size_ = size; recalculateProjection(); }
		float getOrthographicSize()const { return orthographic_size_; }
	private:
		void recalculateProjection();
	private:
		// vertical size
		float orthographic_size_ = 10.0f;
		float orthographic_near_ = -1.0f;
		float orthographic_far_ = 1.0f;
		
		float aspect_ratio_ = 1.0f;
	};
}
#endif