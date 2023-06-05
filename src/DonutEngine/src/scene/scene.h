#ifndef SCENE_H
#define SCENE_H

#include "entt.hpp"

namespace Donut
{
	class Scene
	{
	public:
		Scene();
		~Scene();
	private:
		entt::registry registry_;
	};
}

#endif