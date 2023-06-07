#ifndef SCRIPTABLE_ENTITY_H
#define SCRIPTABLE_ENTITY_H

#include "scene/entity.h"
namespace Donut
{
	class ScriptableEntity
	{
	public:
		template<typename T>
		T& getComponent()
		{
			return entity_.getComponent<T>();
		}


	private:
		Entity entity_;
		friend class Scene;
	};
}
#endif