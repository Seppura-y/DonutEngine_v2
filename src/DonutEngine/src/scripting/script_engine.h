#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

#include <filesystem>

extern"C"
{
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
}

namespace Donut
{
	class ScriptEngine
	{
	public:
		static void init();
		static void shutdown();

		static void loadAssembly(const std::filesystem::path& filepath);
	private:
		static void initMono();
		static void shutdownMono();
		static MonoObject* instantiateClass(MonoClass* mono_class);

		friend class ScriptClass;
	};

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
}

#endif