#include "pch.h"
#include "scene_camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Donut
{
    SceneCamera::SceneCamera()
    {
        recalculateProjection();
    }

    void SceneCamera::setOrthographic(float size, float near_clip, float far_clip)
    {
        orthographic_size_ = size;
        orthographic_near_ = near_clip;
        orthographic_far_ = far_clip;

        recalculateProjection();
    }

    void SceneCamera::setViewportSize(uint32_t width, uint32_t height)
    {
        aspect_ratio_ = (float)width / (float)height;
        recalculateProjection();
    }
    void SceneCamera::recalculateProjection()
    {
        float ortho_left = -orthographic_size_ * aspect_ratio_ * 0.5f;
        float ortho_right = orthographic_size_ * aspect_ratio_ * 0.5f;
        float ortho_bottom = -orthographic_size_ * 0.5f;
        float ortho_top = orthographic_size_ * 0.5f;

        projection_ = glm::ortho(
            ortho_left, ortho_right,
            ortho_bottom, ortho_top,
            orthographic_near_, orthographic_far_);
    }
}