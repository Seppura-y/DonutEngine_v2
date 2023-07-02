#include "pch.h"
#include "script_glue.h"
#include "script_engine.h"

#include "scene/entity.h"
#include "scene/scene.h"

#include "core/uuid.h"

#include "core/key_codes.h"
#include "core/input.h"

#include <mono/metadata/object.h>

namespace Donut
{
#define DN_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Donut.InternalCalls::" #Name, Name)

	static void nativeLog(MonoString* string, int parameter)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		std::cout << str << ", " << parameter << std::endl;
	}

	static void nativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
	{
		DN_CORE_WARN("Value: {0}", (*parameter).x);
		*outResult = glm::normalize(*parameter);
	}

	static float nativeLog_VectorDot(glm::vec3* parameter)
	{
		DN_CORE_WARN("Value: {0}", (*parameter).x);
		return glm::dot(*parameter, *parameter);
	}

	static void entity_getTranslation(UUID uuid, glm::vec3* out_translation)
	{
		Scene* scene = ScriptEngine::getSceneContext();

		
		Entity entity = scene->getEntityByUUID(uuid);
		*out_translation = entity.getComponent<TransformComponent>().translation_;
	}

	static void entity_setTranslation(UUID uuid, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::getSceneContext();

		Entity entity = scene->getEntityByUUID(uuid);
		entity.getComponent<TransformComponent>().translation_ = *translation;
	}

	static bool input_isKeydown(KeyCode keycode)
	{
		return Input::isKeyPressed(keycode);
	}

	void ScriptGlue::registerFunctions()
	{
		//DN_ADD_INTERNAL_CALL(nativeLog);
		//DN_ADD_INTERNAL_CALL(nativeLog_Vector);
		//DN_ADD_INTERNAL_CALL(nativeLog_VectorDot);

		DN_ADD_INTERNAL_CALL(entity_getTranslation);
		DN_ADD_INTERNAL_CALL(entity_setTranslation);

		DN_ADD_INTERNAL_CALL(input_isKeydown);

	}
}