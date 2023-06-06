#include "pch.h"
#include "entity.h"

namespace Donut
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: entity_handle_(handle), scene_(scene)
	{

	}

	Entity::~Entity()
	{
	}
}