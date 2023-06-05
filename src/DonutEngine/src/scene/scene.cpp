#include "pch.h"
#include "scene.h"

#include <glm/glm.hpp>

namespace Donut
{
	static void doMath(const glm::mat4& transform)
	{

	}


	Scene::Scene()
	{
		struct MeshComponent
		{
			//MeshComponent() = default;
			//MeshComponent(const MeshComponent&) = default;

			int a;
		};

		struct TransformComponent
		{
			glm::mat4 transform_;

			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default;
			TransformComponent(const glm::mat4& transform) : transform_(transform){}

			operator glm::mat4&() { return transform_; }
			operator const glm::mat4& () const { return transform_; }
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
			//TransformComponent& transform = group.get<TransformComponent>(entity);
			auto&[trans, mesh] = group.get<TransformComponent, MeshComponent>(entity);
		}

	}

	Scene::~Scene()
	{

	}
}