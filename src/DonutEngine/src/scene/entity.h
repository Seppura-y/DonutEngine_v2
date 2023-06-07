#ifndef ENTITY_H
#define ENTITY_H

#include <entt.hpp>

#include "scene.h"

namespace Donut
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(const Entity&) = default;

		Entity(entt::entity handle, Scene* scene);
		~Entity();

		operator bool() const
		{
			return entity_handle_ != entt::null;
		}

		operator uint32_t() const { return (uint32_t)entity_handle_; }

		bool operator==(const Entity& other) const 
		{
			return entity_handle_ == other.entity_handle_ && scene_ == other.scene_;
		}

		bool operator!=(const Entity& other) const
		{
			return !(other == *this);
		}

		template<typename T>
		bool hasComponent()
		{
			if (scene_)
			{
				return scene_->getRegistry().all_of<T>(entity_handle_);
			}
		}

		template<typename T, typename ...Args>
		T& addComponent(Args&& ...args)
		{
			DN_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!");
			auto& component = scene_->getRegistry().emplace<T>(entity_handle_, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		T& getComponent()
		{
			DN_CORE_ASSERT(!hasComponent<T>(), "Entity does not have this type of component!");
			return scene_->getRegistry().get<T>(entity_handle_);
		}

		template<typename T>
		void removeComponent()
		{
			DN_CORE_ASSERT(!hasComponent<T>(), "Entity does not have this type of component!");
			scene_->getRegistry().remove<T>(entity_handle_);
		}


	private:
		entt::entity entity_handle_{ entt::null };
		Scene* scene_{ nullptr };
	};
}
#endif