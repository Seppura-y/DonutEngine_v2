#ifndef SCENE_H
#define SCENE_H

#include "core/uuid.h"
#include "core/timestep.h"

#include <entt.hpp>

class b2World;

namespace Donut
{
	class Entity;
	class EditorCamera;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void onUpdateRuntime(Timestep ts);
		void onUpdateEditor(Timestep ts, EditorCamera& camera);

		void onViewportResize(uint32_t width, uint32_t height);

		Entity& createEntity(const std::string& tag = std::string());
		Entity& createEntityWithUUID(UUID id, const std::string& tag = std::string());
		entt::entity createEntity();
		entt::registry& getRegistry() { return registry_; }

		void destroyEntity(Entity entity);

		Entity getPrimaryCameraEntity();

		void onRuntimeStart();
		void onRuntimeStop();

		static Ref<Scene> copyScene(Ref<Scene> other);
		void duplicateEntity(Entity entity);
	private:
		template<typename T>
		void onComponentAdded(Entity entity, T& component);
	private:
		entt::registry registry_;
		uint32_t viewport_width_ = 0;
		uint32_t viewport_height_ = 0;

		b2World* physics_world_ = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}

#endif