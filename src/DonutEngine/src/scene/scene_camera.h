#ifndef SCENE_CAMERA_H
#define SCENE_CAMERA_H

#include "renderer/camera.h"
#include <iostream>
namespace Donut
{
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };
	public:
		SceneCamera();
		~SceneCamera() = default;

		void setOrthographic(float size, float near_clip, float far_clip);
		void setPerspective(float fov, float near_clip, float far_clip);

		void setViewportSize(uint32_t width, uint32_t height);


		ProjectionType getProjectionType()const { return projection_type_; }
		void setProjectionType(ProjectionType type) { projection_type_ = type; recalculateProjection(); }
	
		float getOrthographicSize()const { return orthographic_size_; }
		float getOrthographicFarClip() const { return orthographic_far_; }
		float getOrthographicNearClip() const { return orthographic_near_; }

		void setOrthographicSize(float size) { orthographic_size_ = size; recalculateProjection(); }
		void setOrthographicFarClip(float far_clip) { orthographic_far_ = far_clip; recalculateProjection(); }
		void setOrthographicNearClip(float near_clip) { orthographic_near_ = near_clip; recalculateProjection(); }


		float getPerspectiveVerticalFov()const { return perspective_fov_; }
		float getPerspectiveFarClip() const { return perspective_far_; }
		float getPerspectiveNearClip() const { return perspective_near_; }

		void setPerspectiveVerticalFov(float fov) { perspective_fov_ = glm::radians(fov); recalculateProjection(); }
		void setPerspectiveFarClip(float far_clip) { perspective_far_ = far_clip; recalculateProjection(); }
		void setPerspectiveNearClip(float near_clip) { perspective_near_ = near_clip; recalculateProjection(); }
	private:
		void recalculateProjection();
	private:
		ProjectionType projection_type_ = ProjectionType::Orthographic;
		// vertical size
		float orthographic_size_ = 10.0f;
		float orthographic_near_ = -1.0f;
		float orthographic_far_ = 1.0f;

		float perspective_fov_ = glm::radians(45.0f);
		float perspective_near_ = -1.0f;
		float perspective_far_ = 1.0f;
		
		float aspect_ratio_ = 1.0f;
	};
}
#endif