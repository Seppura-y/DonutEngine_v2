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

		Entity createEntity(const std::string& tag = std::string());
		entt::entity createEntity();
		entt::registry& getRegistry() { return registry_; }
	private:
		entt::registry registry_;
	};
}

#endif