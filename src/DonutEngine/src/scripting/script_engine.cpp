#include "pch.h"
#include "script_engine.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

namespace Donut {

	struct ScriptEngineData
	{
		MonoDomain* root_domain_ = nullptr;
		MonoDomain* app_domain_ = nullptr;

		MonoAssembly* core_assemply_ = nullptr;
	};

	static ScriptEngineData* s_data = nullptr;

	void ScriptEngine::init()
	{
		s_data = new ScriptEngineData();

		initMono();
	}

	void ScriptEngine::shutdown()
	{
		shutdownMono();
		delete s_data;
	}

	char* readBytes(const std::string& filepath, uint32_t* outSize)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
		{
			// Failed to open the file
			return nullptr;
		}

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint32_t size = end - stream.tellg();

		if (size == 0)
		{
			// File is empty
			return nullptr;
		}

		char* buffer = new char[size];
		stream.read((char*)buffer, size);
		stream.close();

		*outSize = size;
		return buffer;
	}

	MonoAssembly* loadCSharpAssembly(const std::string& assemblyPath)
	{
		uint32_t file_size = 0;
		char* file_data = readBytes(assemblyPath, &file_size);

		// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(file_data, file_size, 1, &status, 0);

		if (status != MONO_IMAGE_OK)
		{
			const char* error_message = mono_image_strerror(status);
			// Log some error message using the errorMessage data
			return nullptr;
		}

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
		mono_image_close(image);

		// Don't forget to free the file data
		delete[] file_data;

		return assembly;
	}

	void printAssemblyTypes(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* type_definitions_table = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t num_types = mono_table_info_get_rows(type_definitions_table);

		for (int32_t i = 0; i < num_types; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(type_definitions_table, i, cols, MONO_TYPEDEF_SIZE);

			const char* name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			DN_CORE_TRACE("{}.{}", name_space, name);
		}
	}

	void ScriptEngine::initMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* root_domain = mono_jit_init("DountJITRuntime");
		DN_CORE_ASSERT(root_domain, " ");

		// Store the root domain pointer
		s_data->root_domain_ = root_domain;

		// Create an App Domain
		s_data->app_domain_ = mono_domain_create_appdomain("DonutScriptRuntime", nullptr);
		mono_domain_set(s_data->app_domain_, true);

		// Move this maybe
		s_data->core_assemply_ = loadCSharpAssembly("../src/DonutEditor/Resources/Scripts/DonutScriptTest.dll");
		//s_data->core_assemply_ = LoadCSharpAssembly("G:/2023/Code/Project/DonutEngine_v2/src/DonutEditor/Resources/Scripts/DonutScriptTest.dll");
		printAssemblyTypes(s_data->core_assemply_);

		MonoImage* assembly_image = mono_assembly_get_image(s_data->core_assemply_);
		MonoClass* mono_class = mono_class_from_name(assembly_image, "Donut", "Main");

		// 1. create an object (and call constructor)
		MonoObject* instance = mono_object_new(s_data->app_domain_, mono_class);
		mono_runtime_object_init(instance);

		// 2. call function
		MonoMethod* print_message_func = mono_class_get_method_from_name(mono_class, "PrintMessage", 0);
		mono_runtime_invoke(print_message_func, instance, nullptr, nullptr);

		// 3. call function with param
		MonoMethod* print_int_func = mono_class_get_method_from_name(mono_class, "PrintInt", 1);

		int value = 5;
		void* param = &value;

		mono_runtime_invoke(print_int_func, instance, &param, nullptr);

		MonoMethod* print_ints_func = mono_class_get_method_from_name(mono_class, "PrintInts", 2);
		int value2 = 508;
		void* params[2] =
		{
			&value,
			&value2
		};
		mono_runtime_invoke(print_ints_func, instance, params, nullptr);

		MonoString* mono_string = mono_string_new(s_data->app_domain_, "Hello World from C++!");
		MonoMethod* print_custom_message_func = mono_class_get_method_from_name(mono_class, "PrintCustomMessage", 1);
		void* string_param = mono_string;
		mono_runtime_invoke(print_custom_message_func, instance, &string_param, nullptr);

		 //HZ_CORE_ASSERT(false);
	}

	void ScriptEngine::shutdownMono()
	{
		// NOTE(Yan): mono is a little confusing to shutdown, so maybe come back to this

		// mono_domain_unload(s_data->app_domain_);
		s_data->app_domain_ = nullptr;

		// mono_jit_cleanup(s_data->root_domain_);
		s_data->root_domain_ = nullptr;
	}

}