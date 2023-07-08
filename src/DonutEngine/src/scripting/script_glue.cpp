#include "pch.h"
#include "script_glue.h"
#include "script_engine.h"

#include "scene/entity.h"
#include "scene/scene.h"

#include "core/uuid.h"

#include "core/key_codes.h"
#include "core/input.h"

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

#include <box2d/b2_body.h>

namespace Donut
{
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_entity_hasComponent_funcs;

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

	static MonoObject* getScriptInstance(UUID entity_id)
	{
		return ScriptEngine::getManagedInstance(entity_id);
	}

	static bool Entity_hasComponent(UUID entity_id, MonoReflectionType* component_type)
	{
		Scene* scene = ScriptEngine::getSceneContext();
		DN_CORE_ASSERT(scene, "");

		Entity entity = scene->getEntityByUUID(entity_id);
		DN_CORE_ASSERT(entity, "");

		MonoType* mono_component_type = mono_reflection_type_get_type(component_type);
		DN_CORE_ASSERT(s_entity_hasComponent_funcs.find(mono_component_type) != s_entity_hasComponent_funcs.end(), "");
		
		return s_entity_hasComponent_funcs.at(mono_component_type)(entity);
	}

	static uint64_t Entity_findEntityByName(MonoString* name)
	{
		char* name_c_str = mono_string_to_utf8(name);

		Scene* scene = ScriptEngine::getSceneContext();
		DN_CORE_ASSERT(scene, "");
		Entity entity = scene->findEntityByName(name_c_str);

		mono_free(name_c_str);

		if (!entity)
			return 0;

		return entity.getUUID();
	}

	static void TransformComponent_getTranslation(UUID uuid, glm::vec3* out_translation)
	{
		Scene* scene = ScriptEngine::getSceneContext();
		DN_CORE_ASSERT(scene, "");
		
		Entity entity = scene->getEntityByUUID(uuid);
		DN_CORE_ASSERT(entity, "");

		*out_translation = entity.getComponent<TransformComponent>().translation_;
	}

	static void TransformComponent_setTranslation(UUID uuid, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::getSceneContext();
		DN_CORE_ASSERT(scene, "");

		Entity entity = scene->getEntityByUUID(uuid);
		DN_CORE_ASSERT(entity, "");

		entity.getComponent<TransformComponent>().translation_ = *translation;
	}

	static void Rigidbody2DComponent_applyLinearImpulse(UUID uuid, glm::vec2* impulse, glm::vec2* world_pos, bool wake)
	{
		Scene* scene = ScriptEngine::getSceneContext();
		DN_CORE_ASSERT(scene, "");

		Entity entity = scene->getEntityByUUID(uuid);
		DN_CORE_ASSERT(entity, "");

		auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.runtime_body_;
		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(world_pos->x, world_pos->y), wake);
	}

	static void Rigidbody2DComponent_applyLinearImpulseToCenter(UUID uuid, glm::vec2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::getSceneContext();
		DN_CORE_ASSERT(scene, "");

		Entity entity = scene->getEntityByUUID(uuid);
		DN_CORE_ASSERT(entity, "");

		auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.runtime_body_;
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}

	static bool Input_isKeydown(KeyCode keycode)
	{
		return Input::isKeyPressed(keycode);
	}

	void ScriptGlue::registerFunctions()
	{
		//DN_ADD_INTERNAL_CALL(nativeLog);
		//DN_ADD_INTERNAL_CALL(nativeLog_Vector);
		//DN_ADD_INTERNAL_CALL(nativeLog_VectorDot);

		DN_ADD_INTERNAL_CALL(Entity_hasComponent);
		DN_ADD_INTERNAL_CALL(Entity_findEntityByName);

		DN_ADD_INTERNAL_CALL(getScriptInstance);

		DN_ADD_INTERNAL_CALL(TransformComponent_getTranslation);
		DN_ADD_INTERNAL_CALL(TransformComponent_setTranslation);

		DN_ADD_INTERNAL_CALL(Rigidbody2DComponent_applyLinearImpulse);
		DN_ADD_INTERNAL_CALL(Rigidbody2DComponent_applyLinearImpulseToCenter);

		DN_ADD_INTERNAL_CALL(Input_isKeydown);
	}

	template<typename... Component>
	static void registerComponent()
	{
		([]()
		{
			std::string_view type_name = typeid(Component).name();

			size_t pos = type_name.find_last_of(':');
			std::string_view struct_name = type_name.substr(pos + 1);

			std::string managed_type_name = fmt::format("Donut.{}", struct_name);

			DN_CORE_WARN("'{}' = '{}'", type_name, struct_name);

			MonoType* managed_type = mono_reflection_type_from_name(managed_type_name.data(), ScriptEngine::getCoreAssemblyImage());
			if (!managed_type)
			{
				DN_CORE_ERROR("could not find component type {}", managed_type_name);
				return;
			}
			s_entity_hasComponent_funcs[managed_type] = [](Entity entity) { return entity.hasComponent<Component>(); };

		}(), ...);

	}

	template<typename... Component>
	static void registerComponent(ComponentGroup<Component...>)
	{
		registerComponent<Component...>();
	}

	void ScriptGlue::registerComponents()
	{
		s_entity_hasComponent_funcs.clear();
		registerComponent(AllComponents{});
	}
}