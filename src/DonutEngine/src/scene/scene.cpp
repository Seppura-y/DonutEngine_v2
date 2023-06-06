#include "pch.h"
#include "scene.h"
#include "components.h"
#include "renderer/renderer_2d.h"
#include "scene/entity.h"

#include <glm/glm.hpp>

namespace Donut
{
	static void doMath(const glm::mat4& transform)
	{

	}


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

	void Scene::onUpdate(Timestep ts)
	{
		Camera* main_camera = nullptr;
		glm::mat4* camera_transform = nullptr;
		{
			auto group = registry_.view<TransformComponent, CameraComponent>();
			for (auto entity : group)
			{
				auto& [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

				if (camera.is_primary_)
				{
					main_camera = &camera.camera_;
					camera_transform = &transform.transform_;
					break;
				}
			}
		}

		if (main_camera)
		{
			Renderer2D::beginScene(main_camera->getProjection(), *camera_transform);

			auto group = registry_.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::drawRectangle(transform, sprite.color);
			}

			Renderer2D::endScene();
		}
	}

	Entity Scene::createEntity(const std::string& tag)
	{
		Entity entity = { registry_.create(), this };
		entity.addComponent<TagComponent>(tag);
		entity.addComponent<TransformComponent>(glm::mat4(1.0f));

		return entity;
	}

	entt::entity Scene::createEntity()
	{
		return registry_.create();
	}
}