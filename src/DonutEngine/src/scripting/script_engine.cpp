#include "pch.h"
#include "script_engine.h"
#include "script_glue.h"

#include "scene/entity.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

namespace Donut {

	struct ScriptEngineData
	{
		MonoDomain* root_domain_ = nullptr;
		MonoDomain* app_domain_ = nullptr;

		MonoAssembly* core_assemply_ = nullptr;
		MonoImage* core_assembly_image_ = nullptr;

		ScriptClass entity_class_;


		std::unordered_map<std::string, Ref<ScriptClass>> entity_classes_;
		std::unordered_map<UUID, Ref<ScriptInstance>> entity_instances_;

		Scene* scene_context_ = nullptr;
	};

	static ScriptEngineData* s_data = nullptr;

	namespace Utils {

		// todo : move to filesystem class
		static char* readBytes(const std::filesystem::path& filepath, uint32_t* outSize)
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

		static MonoAssembly* loadMonoAssembly(const std::filesystem::path& assembly_path)
		{
			uint32_t file_size = 0;
			char* file_data = readBytes(assembly_path.string(), &file_size);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(file_data, file_size, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* error_message = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}

			std::string path_string = assembly_path.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, path_string.c_str(), &status, 0);
			mono_image_close(image);

			// Don't forget to free the file data
			delete[] file_data;

			return assembly;
		}

		static void printAssemblyTypes(MonoAssembly* assembly)
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
	}

	static void CppFunc()
	{
		std::cout << "this is written in c++\n" << std::endl;
	}

	static void nativeLog(MonoString* string, int value)
	{
		char* c_str = mono_string_to_utf8(string);
		std::string str(c_str);
		mono_free(c_str);
		std::cout << str << value << std::endl;
	}

	static void nativeLogVector(glm::vec3* parameter)
	{
		DN_CORE_WARN("Values : {0}", (*parameter).x);
	}

	void ScriptEngine::init()
	{
		s_data = new ScriptEngineData();

		initMono();
		loadAssembly("../src/DonutEditor/Resources/Scripts/DonutScriptCore.dll");
		loadAssemblyClasses(s_data->core_assemply_);
		// for debug
		Utils::printAssemblyTypes(s_data->core_assemply_);

		ScriptGlue::registerFunctions();
		ScriptGlue::registerComponents();
		
		//MonoObject* instance = s_data->entity_class_.instantiate();
		//MonoMethod* onCreateFunc = s_data->entity_class_.getMethod("onCreate", 0);
		//s_data->entity_class_.invokeMethod(instance, onCreateFunc);
		
		// 1. retrieve and instantiate class (with constructor)
		s_data->entity_class_ = ScriptClass("Donut", "Entity");
#if 0	// example for how to use the API

		// 1. retrieve and instantiate class (with constructor)
		s_data->entity_class_ = ScriptClass("Donut", "Entity");
		
		MonoObject* instance = s_data->entity_class_.instantiate();

		// 2. call method
		MonoMethod* print_message_func = s_data->entity_class_.getMethod("PrintMessage", 0);
		s_data->entity_class_.invokeMethod(instance, print_message_func);

		// 3. call method with param
		MonoMethod* print_int_func = s_data->entity_class_.getMethod("PrintInt", 1);

		int value = 5;
		void* param = &value;

		s_data->entity_class_.invokeMethod(instance, print_int_func, &param);

		MonoMethod* print_ints_func = s_data->entity_class_.getMethod("PrintInts", 2);
		int value2 = 508;
		void* params[2] =
		{
			&value,
			&value2
		};
		s_data->entity_class_.invokeMethod(instance, print_ints_func, params);


		MonoString* mono_string = mono_string_new(s_data->app_domain_, "Hello World from C++!");


		MonoMethod* print_custom_message_func = s_data->entity_class_.getMethod("PrintCustomMessage", 1);
		void* string_param = mono_string;
		s_data->entity_class_.invokeMethod(instance, print_custom_message_func, &string_param);

		//HZ_CORE_ASSERT(false);
#endif
	}

	void ScriptEngine::shutdown()
	{
		shutdownMono();
		delete s_data;
	}

	static void nativeLogVectorRet(glm::vec3* parameter, glm::vec3* out)
	{
		*out = glm::normalize(*parameter);
		//*out = glm::cross(*parameter, glm::vec3(1, 1, 1));
		//*out = glm::cross(*parameter, glm::vec3(parameter->x, parameter->y, parameter->z));
	}

	void ScriptEngine::initMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* root_domain = mono_jit_init("DountJITRuntime");
		DN_CORE_ASSERT(root_domain, " ");

		// Store the root domain pointer
		s_data->root_domain_ = root_domain;
	}

	void ScriptEngine::shutdownMono()
	{
		// NOTE(Yan): mono is a little confusing to shutdown, so maybe come back to this

		// mono_domain_unload(s_data->app_domain_);
		s_data->app_domain_ = nullptr;

		// mono_jit_cleanup(s_data->root_domain_);
		s_data->root_domain_ = nullptr;
	}


	void ScriptEngine::loadAssembly(const std::filesystem::path& filepath)
	{
		// Create an App Domain
		s_data->app_domain_ = mono_domain_create_appdomain("DonutScriptRuntime", nullptr);
		mono_domain_set(s_data->app_domain_, true);

		s_data->core_assemply_ = Utils::loadMonoAssembly(filepath);

		s_data->core_assembly_image_ = mono_assembly_get_image(s_data->core_assemply_);
	}

	void ScriptEngine::onRuntimeStart(Scene* scene)
	{
		s_data->scene_context_ = scene;
	}

	void ScriptEngine::onRuntimeStop()
	{
		s_data->scene_context_ = nullptr;
		s_data->entity_instances_.clear();
	}

	bool ScriptEngine::isClassExists(const std::string& full_classname)
	{
		return s_data->entity_classes_.find(full_classname) != s_data->entity_classes_.end();
	}

	void ScriptEngine::onCreateEntity(Entity entity)
	{
		const auto& sc = entity.getComponent<ScriptComponent>();
		if (ScriptEngine::isClassExists(sc.class_name_))
		{
			Ref<ScriptInstance> instance = createRef<ScriptInstance>(s_data->entity_classes_[sc.class_name_], entity);
			s_data->entity_instances_[entity.getUUID()] = instance;

			instance->invokeOnCreate();
		}
	}

	void ScriptEngine::onUpdateEntity(Entity entity, float ts)
	{
		UUID entity_uuid = entity.getUUID();
		DN_CORE_ASSERT(s_data->entity_instances_.find(entity_uuid) != s_data->entity_instances_.end(), "");

		Ref<ScriptInstance> instance = s_data->entity_instances_[entity_uuid];
		instance->invokeOnUpdate(ts);
	}

	Scene* ScriptEngine::getSceneContext()
	{
		return s_data->scene_context_;
	}

	std::unordered_map<std::string, Ref<ScriptClass>> ScriptEngine::getEntityClasses()
	{
		return s_data->entity_classes_;
	}

	MonoObject* ScriptEngine::instantiateClass(MonoClass* mono_class)
	{
		//  create an object (and call constructor)
		MonoObject* instance = mono_object_new(s_data->app_domain_, mono_class);
		mono_runtime_object_init(instance);
		return instance;
	}

	void ScriptEngine::loadAssemblyClasses(MonoAssembly* assembly)
	{
		s_data->entity_classes_.clear();

		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* type_definitions_table = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t num_types = mono_table_info_get_rows(type_definitions_table);

		MonoClass* entity_class = mono_class_from_name(s_data->core_assembly_image_, "Donut", "Entity");

		for (int32_t i = 0; i < num_types; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(type_definitions_table, i, cols, MONO_TYPEDEF_SIZE);

			const char* name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			std::string full_name;
			if (strlen(name_space) != 0)
			{
				full_name = fmt::format("{}.{}", name_space, name);
			}
			else
			{
				full_name = name;
			}

			MonoClass* mono_class = mono_class_from_name(s_data->core_assembly_image_, name_space, name);

			if (mono_class == entity_class)
			{
				continue;
			}

			bool is_entity = mono_class_is_subclass_of(mono_class, entity_class, false);
			if (is_entity)
			{
				s_data->entity_classes_[full_name] = createRef<ScriptClass>(name_space, name);
			}
		}
	}

	MonoImage* ScriptEngine::getCoreAssemblyImage()
	{
		return s_data->core_assembly_image_;
	}

	ScriptClass::ScriptClass(const std::string& class_namespace, const std::string& class_name)
		:class_namespace_(class_namespace), class_name_(class_name)
	{
		mono_class_ = mono_class_from_name(s_data->core_assembly_image_, class_namespace_.c_str(), class_name_.c_str());
	}

	MonoObject* ScriptClass::instantiate()
	{
		return ScriptEngine::instantiateClass(mono_class_);
	}

	MonoMethod* ScriptClass::getMethod(const std::string& name, int parameter_count)
	{
		return mono_class_get_method_from_name(mono_class_, name.c_str(), parameter_count);
	}

	MonoObject* ScriptClass::invokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		return mono_runtime_invoke(method, instance, params, nullptr);
	}





	ScriptInstance::ScriptInstance(Ref<ScriptClass> sricpt_class, Entity entity)
		: script_class_(sricpt_class)
	{
		instance_ = script_class_->instantiate();

		//constructor_ = script_class_->getMethod(".ctor", 1);
		
		// need to get the constructor from the parent class (Entity)
		constructor_ = s_data->entity_class_.getMethod(".ctor", 1);
		onCreateMethod_ = script_class_->getMethod("onCreate", 0);
		onUpdateMethod_ = script_class_->getMethod("onUpdate", 1);

		{
			UUID entity_uuid = entity.getUUID();
			void* param = &entity_uuid;
			script_class_->invokeMethod(instance_, constructor_, &param);
		}
	}

	void ScriptInstance::invokeOnCreate()
	{
		script_class_->invokeMethod(instance_, onCreateMethod_, nullptr);
	}

	void ScriptInstance::invokeOnUpdate(float ts)
	{
		void* param = &ts;
		script_class_->invokeMethod(instance_, onUpdateMethod_, &param);
	}
}