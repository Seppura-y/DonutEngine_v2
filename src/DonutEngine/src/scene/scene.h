#ifndef SCENE_H
#define SCENE_H

#include "entt.hpp"
#include "core/timestep.h"

namespace Donut
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void onUpdate(Timestep ts);
		void onViewportResize(uint32_t width, uint32_t height);

		Entity createEntity(const std::string& tag = std::string());
		entt::entity createEntity();
		entt::registry& getRegistry() { return registry_; }

		void destroyEntity(Entity entity);

		Entity getPrimaryCameraEntity();
	private:
		template<typename T>
		void onComponentAdded(Entity entity, T& component);
	private:
		entt::registry registry_;
		uint32_t viewport_width_ = 0;
		uint32_t viewport_height_ = 0;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}

#endif