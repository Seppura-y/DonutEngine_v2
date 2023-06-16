#include "pch.h"
#include "scene.h"
#include "components.h"
#include "renderer/renderer_2d.h"

#include "renderer/editor_camera.h"

#include "scene/entity.h"


#include <glm/glm.hpp>

namespace Donut
{
	Scene::Scene()
	{
#if ENTT_TEST_CODE
		struct MeshComponent
		{
			//MeshComponent() = default;
			//MeshComponent(const MeshComponent&) = default;

			int a;
		};


		entt::entity entity = registry_.create();
		registry_.emplace<TransformComponent>(entity, glm::mat4(1.0f));
		
		// 这个entity中是否存在TransformComponent
		if (registry_.all_of<TransformComponent>(entity))
		{
			TransformComponent& transform = registry_.get<TransformComponent>(entity);
		}

		// 获取所有存在TransformComponent的entity
		auto view = registry_.view<TransformComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = registry_.get<TransformComponent>(entity);
		}

		// 
		auto group = registry_.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group)
		{
			auto&[trans, mesh] = group.get<TransformComponent, MeshComponent>(entity);
		}
#endif
	}

	Scene::~Scene()
	{

	}

	void Scene::onUpdateRuntime(Timestep ts)
	{
		// Update scripts
		{
			registry_.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
				{
					if (!nsc.instance_)
					{
						nsc.instance_ = nsc.instantiateScript();
						nsc.instance_->entity_ = Entity{ entity, this };
						nsc.instance_->onCreate();
					}

					nsc.instance_->onUpdate(ts);
				});
		}

		Camera* main_camera = nullptr;
		glm::mat4 camera_transform;
		{
			auto group = registry_.view<TransformComponent, CameraComponent>();
			for (auto entity : group)
			{
				auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

				if (camera.is_primary_)
				{
					main_camera = &camera.camera_;
					camera_transform = transform.getTransform();
					break;
				}
			}
		}

		if (main_camera)
		{
			Renderer2D::beginScene(main_camera->getProjection(), camera_transform);

			auto group = registry_.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				// return a tuple that consturct with the references value, and the tuple return as value
				// so we don't need to use '&' to receive the return value.
				// because the API already did that.
				//auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				//Renderer2D::drawRectangle(transform.getTransform(), sprite.color_);
				Renderer2D::drawSprite(transform.getTransform(), sprite, (int)entity);
			}

			Renderer2D::endScene();
		}
	}

	void Scene::onUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::beginScene(camera);

		auto group = registry_.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			// return a tuple that consturct with the references value, and the tuple return as value
			// so we don't need to use '&' to receive the return value.
			// because the API already did that.
			//auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			//Renderer2D::drawRectangle(transform.getTransform(), sprite.color_);
			Renderer2D::drawSprite(transform.getTransform(), sprite, (int)entity);
		}

		Renderer2D::endScene();
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height)
	{
		viewport_width_ = width;
		viewport_height_ = height;

		// resize our non fixed aspect ratio camera
		auto view = registry_.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& camera_component = view.get<CameraComponent>(entity);
			if (!camera_component.is_fixed_aspect_ratio_)
			{
				camera_component.camera_.setViewportSize(width, height);
			}
		}
	}

	Entity Scene::createEntity(const std::string& tag)
	{
		Entity entity = { registry_.create(), this };
		entity.addComponent<TransformComponent>();

		auto t = entity.addComponent<TagComponent>(tag);
		t.tag_ = tag.empty() ? "entity" : tag;
		return entity;
	}

	entt::entity Scene::createEntity()
	{
		return registry_.create();
	}
	void Scene::destroyEntity(Entity entity)
	{
		registry_.destroy(entity);
	}

	Entity Scene::getPrimaryCameraEntity()
	{
		auto view = registry_.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.is_primary_)
			{
				return Entity{ entity, this };
			}
		}

		return {};
	}

	template<typename T>
	void Scene::onComponentAdded(Entity entity, T& component)
	{
		// you never suppose to not have a specializaion for this function
		static_assert(false);
	}

	template<>
	void Scene::onComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{

	}

	template<>
	void Scene::onComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		//component.camera_.setViewportSize(viewport_width_, viewport_height_);
		if (viewport_width_ > 0 && viewport_height_ > 0)
		{
			component.camera_.setViewportSize(viewport_width_, viewport_height_);
		}
	}

	template<>
	void Scene::onComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{

	}

	template<>
	void Scene::onComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{

	}

	template<>
	void Scene::onComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{

	}

}