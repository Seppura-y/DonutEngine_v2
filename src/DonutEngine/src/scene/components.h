#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>
#include "renderer/texture.h"
#include "renderer/camera.h"
#include "scene/scene_camera.h"

namespace Donut
{
	struct TagComponent
	{
		std::string tag_;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: tag_(tag)
		{

		}
	};

	struct TransformComponent
	{
		glm::mat4 transform_{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform) : transform_(transform) {}

		operator glm::mat4& () { return transform_; }
		operator const glm::mat4& () const { return transform_; }
	};

	struct SpriteRendererComponent
	{
		glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> texture;
		float tiling_factor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& c) :color(c) {}

	};

	struct CameraComponent
	{
		Donut::SceneCamera camera_;
		bool is_primary_ = true;
		bool is_fixed_aspect_ratio_ = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};
}
#endif