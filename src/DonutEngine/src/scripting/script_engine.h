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



	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& class_namespace, const std::string& class_name, bool is_core = false);

		MonoObject* instantiate();
		MonoMethod* getMethod(const std::string& name, int parameter_count);
		MonoObject* invokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		//std::map<std::string, ScriptField>& getFields() const { return fields_; }
	private:
		std::string class_name_;
		std::string class_namespace_;
		MonoClass* mono_class_ = nullptr;

		//std::map<std::string, ScriptField> fields_;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> sricpt_class, Entity entity);

		void invokeOnCreate();
		void invokeOnUpdate(float ts);

	private:
		Ref<ScriptClass> script_class_;

		MonoObject* instance_ = nullptr;
		MonoMethod* constructor_ = nullptr;
		MonoMethod* onCreateMethod_ = nullptr;
		MonoMethod* onUpdateMethod_ = nullptr;
	};

	class ScriptEngine
	{
	public:
		static void init();
		static void shutdown();

		static void loadAssembly(const std::filesystem::path& filepath);
		static void loadAppAssembly(const std::filesystem::path& filepath);

		static void onRuntimeStart(Scene* scene);
		static void onRuntimeStop();

		static bool isClassExists(const std::string& full_classname);

		static void onCreateEntity(Entity entity);
		static void onUpdateEntity(Entity entity, float ts);

		static Scene* getSceneContext();

		static std::unordered_map<std::string, Ref<ScriptClass>> getEntityClasses();

		static MonoImage* getCoreAssemblyImage();
	private:
		static void initMono();
		static void shutdownMono();
		static MonoObject* instantiateClass(MonoClass* mono_class);

		static void loadAssemblyClasses();


		friend class ScriptClass;
		friend class ScriptGlue;
	};


}

#endif