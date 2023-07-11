#ifndef PHYSICS_2D_H
#define PHYSICS_2D_H

#include "core/core.h"
#include "scene/components.h"

#include "box2d/b2_body.h"

namespace Donut
{
	namespace Utils
	{
		inline b2BodyType rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType body_type)
		{
			switch (body_type)
			{
			case Rigidbody2DComponent::BodyType::Static:    return b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic:   return b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
			}

			DN_CORE_ASSERT(false, "Unknown body type");
			return b2_staticBody;
		}

		inline Rigidbody2DComponent::BodyType rigidbody2DTypeFromBox2DBody(b2BodyType body_type)
		{
			switch (body_type)
			{
			case b2_staticBody:    return Rigidbody2DComponent::BodyType::Static;
			case b2_dynamicBody:   return Rigidbody2DComponent::BodyType::Dynamic;
			case b2_kinematicBody: return Rigidbody2DComponent::BodyType::Kinematic;
			}

			DN_CORE_ASSERT(false, "Unknown body type");
			return Rigidbody2DComponent::BodyType::Static;
		}
	}
}
#endif