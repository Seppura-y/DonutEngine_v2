#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

#include <filesystem>

#include "scene/scene.h"

extern"C"
{
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
}

namespace Donut
{
	enum class ScriptFieldType
	{
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity
	};

	struct ScriptField
	{
		ScriptFieldType type_;
		std::string name_;

		MonoClassField* mono_class_field_;
	};

	struct ScriptFieldInstance
	{
		ScriptField field_;

		ScriptFieldInstance()
		{
			memset(buffer_, 0, sizeof(buffer_));
		}

		template<typename T>
		T getValue()
		{
			static_assert(sizeof(T) <= 16, "Type too large!");
			return *(T*)buffer_;
		}

		template<typename T>
		void setValue(T value)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");
			memcpy(buffer_, &value, sizeof(T));
		}
	private:
		uint8_t buffer_[16];

		friend class ScriptEngine;
		friend class ScriptInstance;
	};

	using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& class_namespace, const std::string& class_name, bool is_core = false);

		MonoObject* instantiate();
		MonoMethod* getMethod(const std::string& name, int parameter_count);
		MonoObject* invokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		const std::map<std::string, ScriptField>& getFields() const { return fields_; }
	private:
		std::string class_name_;
		std::string class_namespace_;
		MonoClass* mono_class_ = nullptr;

		std::map<std::string, ScriptField> fields_;

		friend class ScriptEngine;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> sricpt_class, Entity entity);

		Ref<ScriptClass> getScriptClass() { return script_class_; }

		void invokeOnCreate();
		void invokeOnUpdate(float ts);

		template<typename T>
		T getFieldValue(const std::string& name)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");

			bool success = getFieldValueInternal(name, field_value_buffer_);
			if (!success)
			{
				return T();
			}
			return *(T*)field_value_buffer_;
		}


		template<typename T>
		void setFieldValue(const std::string& name, T value)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");

			setFieldValueInternal(name, &value);
		}

		MonoObject* getManagedObject() { return instance_; }
	private:
		bool getFieldValueInternal(const std::string& name, void* buffer);
		bool setFieldValueInternal(const std::string& name, const void* buffer);
	private:
		Ref<ScriptClass> script_class_;

		MonoObject* instance_ = nullptr;
		MonoMethod* constructor_ = nullptr;
		MonoMethod* onCreateMethod_ = nullptr;
		MonoMethod* onUpdateMethod_ = nullptr;

		inline static char field_value_buffer_[16];

		friend class ScriptEngine;
		friend class ScriptFieldInstance;
	};

	class ScriptEngine
	{
	public:
		static void init();
		static void shutdown();

		static bool loadAssembly(const std::filesystem::path& filepath);
		static bool loadAppAssembly(const std::filesystem::path& filepath);

		static void reloadAssembly();

		static void onRuntimeStart(Scene* scene);
		static void onRuntimeStop();

		static bool isClassExists(const std::string& full_classname);

		static void onCreateEntity(Entity entity);
		static void onUpdateEntity(Entity entity, float ts);

		static Scene* getSceneContext();
		static Ref<ScriptInstance> getEntityScriptInstance(UUID entity_id);

		static std::unordered_map<std::string, Ref<ScriptClass>> getEntityClasses();

		static Ref<ScriptClass> getEntityClass(const std::string& name);
		static ScriptFieldMap& getScriptFieldMap(Entity entity);

		static MonoImage* getCoreAssemblyImage();

		static MonoObject* getManagedInstance(UUID uuid);
	private:
		static void initMono();
		static void shutdownMono();
		static MonoObject* instantiateClass(MonoClass* mono_class);

		static void loadAssemblyClasses();


		friend class ScriptClass;
		friend class ScriptGlue;
	};

	namespace Utils {

		inline const char* scriptFieldTypeToString(ScriptFieldType fieldType)
		{
			switch (fieldType)
			{
			case ScriptFieldType::None:    return "None";
			case ScriptFieldType::Float:   return "Float";
			case ScriptFieldType::Double:  return "Double";
			case ScriptFieldType::Bool:    return "Bool";
			case ScriptFieldType::Char:    return "Char";
			case ScriptFieldType::Byte:    return "Byte";
			case ScriptFieldType::Short:   return "Short";
			case ScriptFieldType::Int:     return "Int";
			case ScriptFieldType::Long:    return "Long";
			case ScriptFieldType::UByte:   return "UByte";
			case ScriptFieldType::UShort:  return "UShort";
			case ScriptFieldType::UInt:    return "UInt";
			case ScriptFieldType::ULong:   return "ULong";
			case ScriptFieldType::Vector2: return "Vector2";
			case ScriptFieldType::Vector3: return "Vector3";
			case ScriptFieldType::Vector4: return "Vector4";
			case ScriptFieldType::Entity:  return "Entity";
			}
			DN_CORE_ASSERT(false, "Unknown ScriptFieldType");
			return "None";
		}

		inline ScriptFieldType scriptFieldTypeFromString(std::string_view fieldType)
		{
			if (fieldType == "None")    return ScriptFieldType::None;
			if (fieldType == "Float")   return ScriptFieldType::Float;
			if (fieldType == "Double")  return ScriptFieldType::Double;
			if (fieldType == "Bool")    return ScriptFieldType::Bool;
			if (fieldType == "Char")    return ScriptFieldType::Char;
			if (fieldType == "Byte")    return ScriptFieldType::Byte;
			if (fieldType == "Short")   return ScriptFieldType::Short;
			if (fieldType == "Int")     return ScriptFieldType::Int;
			if (fieldType == "Long")    return ScriptFieldType::Long;
			if (fieldType == "UByte")   return ScriptFieldType::UByte;
			if (fieldType == "UShort")  return ScriptFieldType::UShort;
			if (fieldType == "UInt")    return ScriptFieldType::UInt;
			if (fieldType == "ULong")   return ScriptFieldType::ULong;
			if (fieldType == "Vector2") return ScriptFieldType::Vector2;
			if (fieldType == "Vector3") return ScriptFieldType::Vector3;
			if (fieldType == "Vector4") return ScriptFieldType::Vector4;
			if (fieldType == "Entity")  return ScriptFieldType::Entity;

			DN_CORE_ASSERT(false, "Unknown ScriptFieldType");
			return ScriptFieldType::None;
		}

	}
}

#endif