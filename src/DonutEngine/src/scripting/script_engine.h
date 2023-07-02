#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

#include <filesystem>

extern"C"
{
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
}

namespace Donut
{
	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& class_namespace, const std::string& class_name);

		MonoObject* instantiate();
		MonoMethod* getMethod(const std::string& name, int parameter_count);
		MonoObject* invokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

	private:
		std::string class_name_;
		std::string class_namespace_;
		MonoClass* mono_class_ = nullptr;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> sricpt_class);

		void invokeOnCreate();
		void invokeOnUpdate(float ts);

	private:
		Ref<ScriptClass> script_class_;

		MonoObject* instance_ = nullptr;
		MonoMethod* onCreateMethod_ = nullptr;
		MonoMethod* onUpdateMethod_ = nullptr;
	};

	class ScriptEngine
	{
	public:
		static void init();
		static void shutdown();

		static void loadAssembly(const std::filesystem::path& filepath);

		static std::unordered_map<std::string, Ref<ScriptClass>> getEntityClasses();
	private:
		static void initMono();
		static void shutdownMono();
		static MonoObject* instantiateClass(MonoClass* mono_class);
		static void loadAssemblyClasses(MonoAssembly* assembly);

		friend class ScriptClass;
	};


}

#endif