#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "scene/scene_camera.h"
#include "core/uuid.h"

#include "renderer/font.h"

#include "renderer/texture.h"
#include "renderer/camera.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


namespace Donut
{
	struct IDComponent
	{
		UUID id_;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

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
		glm::vec3 translation_ = { 0.0f,0.0f,0.0f };
		glm::vec3 rotation_ = { 0.0f,0.0f,0.0f };
		glm::vec3 scale_ = { 1.0f,1.0f,1.0f };

		//glm::mat4 transform_{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation) : translation_(translation){}

		//TransformComponent(const glm::mat4& transform) : transform_(transform) {}

		//operator glm::mat4& () { return transform_; }
		//operator const glm::mat4& () const { return transform_; }

		glm::mat4 getTransform() const
		{
			//glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rotation_.x, { 1, 0, 0 })
			//	* glm::rotate(glm::mat4(1.0f), rotation_.y, { 0, 1, 0 })
			//	* glm::rotate(glm::mat4(1.0f), rotation_.z, { 0, 0, 1 });

			glm::mat4 rotation = glm::toMat4(glm::quat(rotation_));

			glm::mat4 scale = glm::scale(glm::mat4(1.0f), scale_);

			return glm::translate(glm::mat4(1.0f), translation_)
				* rotation
				* scale;
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 color_{ 0.8f, 0.2f, 0.3f, 1.0f };
		AssetHandle texture_ = 0;
		float tiling_factor_ = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& c) :color_(c) {}

	};

	struct CircleRendererComponent
	{
		glm::vec4 color_{ 0.6f, 0.4f, 0.2f, 1.0f };
		float thickness_ = 1.0f;
		float fade_ = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};

	struct CameraComponent
	{
		Donut::SceneCamera camera_;
		bool is_primary_ = true;
		bool is_fixed_aspect_ratio_ = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	class ScriptableEntity;
	struct NativeScriptComponent_Version1
	{
		ScriptableEntity* instance_ = nullptr;

		std::function<void()> instantiateFunction;
		std::function<void()> destroyInstanceFunction;

		std::function<void(ScriptableEntity*)> onCreateFunction;
		std::function<void(ScriptableEntity*)> onDestroyFunction;
		std::function<void(ScriptableEntity*, Timestep)> onUpdateFunction;

		template<typename T>
		void bind()
		{
			instantiateFunction = [&]() { instance_ = new T(); };
			destroyInstanceFunction = [&]() {delete (T*)instance_; instance_ = nullptr; };

			onCreateFunction = [](ScriptableEntity* instance) { ((T*)instance)->onCreate(); };
			onDestroyFunction = [](ScriptableEntity* instance) { ((T*)instance)->onDestroy(); };
			onUpdateFunction = [](ScriptableEntity* instance, Timestep ts) { ((T*)instance)->onUpdate(ts); };
		}
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* instance_ = nullptr;

		ScriptableEntity* (*instantiateScript)();
		void (*destroyScript)(NativeScriptComponent*);

		template<typename T>
		void bind()
		{
			instantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			destroyScript = [](NativeScriptComponent* nsc) {delete nsc->instance_; nsc->instance_ = nullptr; };
		}
	};


	struct Rigidbody2DComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType type_ = BodyType::Static;
		bool fixed_rotation_ = false;

		// Storage for runtime
		void* runtime_body_ = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 offset_ = { 0.0f, 0.0f };
		glm::vec2 size_ = { 0.5f, 0.5f };

		// TODO(Yan): move into physics material in the future maybe
		float density_ = 1.0f;					//密度
		float friction_ = 0.5f;					//摩擦
		float restitution_ = 0.0f;				//弹性系数
		float restitution_threshold_ = 0.5f;	//反弹触发阈值？

		// Storage for runtime
		void* runtime_fixture_ = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct CircleCollider2DComponent
	{
		glm::vec2 offset_ = { 0.0f, 0.0f };
		float radius_ = 0.5f;

		float density_ = 1.0f;
		float friction_ = 0.5f;
		float restitution_ = 0.0f;
		float restitution_threshold_ = 0.5f;

		void* runtime_fixture_ = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

	struct ScriptComponent
	{
		std::string class_name_;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent& other) = default;
	};

	struct TextComponent
	{
		std::string text_string_;
		float kerning_ = 0.0f;
		float line_spacing_ = 0.0f;
		Ref<Font> font_ = Font::getDefaultFont();
		glm::vec4 color_ = glm::vec4(1.0f);
	};

	 template<typename... Component>
	 struct ComponentGroup
	 {

	 };

	 using AllComponents =
			ComponentGroup<TransformComponent, SpriteRendererComponent, CircleRendererComponent, CameraComponent,ScriptComponent,
							CircleCollider2DComponent, BoxCollider2DComponent, NativeScriptComponent, Rigidbody2DComponent, TextComponent>;
}
#endif