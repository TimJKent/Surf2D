#include "sepch.h"
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "SurfEngine/Scripting/ScriptFuncs.h"
#include "SurfEngine/Scripting/ScriptGlue.h"

namespace SurfEngine {

	namespace Utils {

		// TODO: move to FileSystem class
		static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream)
			{
				// Failed to open the file
				return nullptr;
			}

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint64_t size = end - stream.tellg();

			if (size == 0)
			{
				// File is empty
				return nullptr;
			}

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = (uint32_t)size;
			return buffer;
		}

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath, &fileSize);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}

			std::string pathString = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
			mono_image_close(image);

			// Don't forget to free the file data
			delete[] fileData;

			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
				SE_CORE_TRACE("{}.{}", nameSpace, name);
			}
		}

	}

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppAssemblyImage = nullptr;

		ScriptClass ObjectClass;

		std::unordered_map<std::string, Ref<ScriptClass>> ObjectClasses;
		std::unordered_map<UUID, Ref<ScriptInstance>> ObjectInstances;

		// Runtime
		Scene* SceneContext = nullptr;
	};

	static ScriptEngineData* s_ScriptData = nullptr;
	ScriptEngineStorage* ScriptEngine::s_Storage = nullptr;


	// SCRIPT ENGINE

	void ScriptEngine::Init()
	{
		//Init Storage
		s_ScriptData = new ScriptEngineData();
		s_Storage = new ScriptEngineStorage();

		mono_set_assemblies_path("../SurfEngine/vendor/mono/lib");

		//Init Mono RunTime
		MonoDomain* rootDomain = mono_jit_init("SurfEngineJITRuntime");
		SE_CORE_ASSERT(rootDomain);
		s_ScriptData->RootDomain = rootDomain;
	}
	
	void ScriptEngine::Shutdown()
	{
		if (!s_ScriptData) { return; }
		mono_domain_set(s_ScriptData->RootDomain, false);

		mono_domain_unload(s_ScriptData->AppDomain);
		s_ScriptData->AppDomain = nullptr;

		mono_jit_cleanup(s_ScriptData->RootDomain);
		s_ScriptData->RootDomain = nullptr;

		delete s_ScriptData;
		delete s_Storage;
	}

	void ScriptEngine::CreateDomain()
	{
		MonoDomain* newDomain = mono_domain_create_appdomain("SurfEngineScriptRuntime", NULL);
		//mono_thread_push_appdomain_ref(newDomain);
		if (!mono_domain_set(newDomain, false))
		{
			SE_CORE_ASSERT();
		}
		s_ScriptData->AppDomain = newDomain;
	}

	void ScriptEngine::UnloadDomain()
	{
		MonoDomain* domainToUnload = mono_domain_get();
		if (domainToUnload && domainToUnload != mono_get_root_domain())
		{
			mono_domain_set(s_ScriptData->RootDomain, true);
			//mono_thread_pop_appdomain_ref();
			mono_domain_unload(domainToUnload);
		}
		//mono_gc_collect(mono_gc_max_generation());
	}
	
	void ScriptEngine::LoadCoreAssembly(const std::filesystem::path& filepath)
	{
		// Move this maybe
		s_ScriptData->CoreAssembly = Utils::LoadMonoAssembly(filepath);
		s_ScriptData->CoreAssemblyImage = mono_assembly_get_image(s_ScriptData->CoreAssembly);
		// Utils::PrintAssemblyTypes(s_ScriptData->CoreAssembly);
	}

	void ScriptEngine::LoadAppAssembly(const std::filesystem::path& filepath)
	{
		// Move this maybe
		s_ScriptData->AppAssembly = Utils::LoadMonoAssembly(filepath);
		auto assemb = s_ScriptData->AppAssembly;
		s_ScriptData->AppAssemblyImage = mono_assembly_get_image(s_ScriptData->AppAssembly);
		auto assembi = s_ScriptData->AppAssemblyImage;
	    // Utils::PrintAssemblyTypes(s_ScriptData->AppAssembly);
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
		CreateDomain();
		LoadCoreAssembly("..\\..\\Surf2D\\EngineEditor\\res\\Scripts\\SurfScript-Core.dll");

		ScriptGlue::RegisterComponents();
		ScriptGlue::RegisterFunctions();

		s_ScriptData->ObjectClass = ScriptClass("SurfEngine", "Script", true);
		LoadAppAssembly("..\\..\\Surf2D\\bin\\Debug-windows-x86_64\\EngineEditor\\UserScript.dll");
		LoadAssemblyClasses();

		s_ScriptData->SceneContext = scene;
	}

	void ScriptEngine::OnRuntimeStop()
	{
		UnloadDomain();
		s_ScriptData->SceneContext = nullptr;
		s_ScriptData->ObjectInstances.clear();
	}

	bool ScriptEngine::ObjectClassExists(const std::string& fullClassName)
	{
		return s_ScriptData->ObjectClasses.find(fullClassName) != s_ScriptData->ObjectClasses.end();
	}

	void ScriptEngine::OnCreateObject(Object object)
	{
		const auto& sc = object.GetComponent<ScriptComponent>();
		std::filesystem::path classpath = sc.path;
		std::string className = classpath.stem().string();

		if (ScriptEngine::ObjectClassExists(className))
		{
			Ref<ScriptInstance> instance = std::make_shared<ScriptInstance>(s_ScriptData->ObjectClasses[className], object);
			s_ScriptData->ObjectInstances[object.GetComponent<TagComponent>().uuid] = instance;
			instance->InvokeOnCreate();
		}
	}

	void ScriptEngine::OnUpdateObject(Object object, Timestep ts)
	{
		UUID objectUUID = object.GetComponent<TagComponent>().uuid;
		SE_CORE_ASSERT(s_ScriptData->ObjectInstances.find(objectUUID) != s_ScriptData->ObjectInstances.end());
		Ref<ScriptInstance> instance = s_ScriptData->ObjectInstances[objectUUID];
		instance->InvokeOnUpdate((float)ts);
	}

	Scene* ScriptEngine::GetSceneContext()
	{
		return s_ScriptData->SceneContext;
	}

	std::unordered_map<std::string, Ref<ScriptClass>> ScriptEngine::GetObjectClasses()
	{
		return s_ScriptData->ObjectClasses;
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		s_ScriptData->ObjectClasses.clear();

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_ScriptData->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* objectClass = mono_class_from_name(s_ScriptData->CoreAssemblyImage, "SurfEngine", "Script");
		SE_CORE_ASSERT(objectClass);
		
		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_ScriptData->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(s_ScriptData->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
			std::string fullName;
			if (strlen(nameSpace) != 0)
				fullName = fmt::format("{}.{}", nameSpace, name);
			else
				fullName = name;

			MonoClass* monoClass = mono_class_from_name(s_ScriptData->AppAssemblyImage, nameSpace, name);

			if (monoClass == objectClass)
				continue;

			if (monoClass)
			{
				bool isObject = mono_class_is_subclass_of(monoClass, objectClass, false);
				if (isObject)
					s_ScriptData->ObjectClasses[fullName] = std::make_shared<ScriptClass>(nameSpace, name);
			}
		}
	}

	MonoImage* ScriptEngine::GetCoreAssemblyImage()
	{
		return s_ScriptData->CoreAssemblyImage;
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_ScriptData->AppDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	
	//Script Class
	
	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(isCore ? s_ScriptData->CoreAssemblyImage : s_ScriptData->AppAssemblyImage, classNamespace.c_str(), className.c_str());
	}

	MonoObject* ScriptClass::Instantiate()
	{
		return ScriptEngine::InstantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		if (instance) {
			if (method) {
				if (params) {
					MonoObject* exception;
					MonoObject* returnvalue;

					returnvalue = mono_runtime_invoke(method, instance, params, &exception);


					if (exception) {
						MonoClass* klass = mono_object_get_class(instance);

						std::string method_name = mono_method_get_name(method);
						std::string class_name = mono_class_get_name(klass);


						char* cStr = mono_string_to_utf8(mono_object_to_string(exception, nullptr));
						std::string error(cStr);
						mono_free(cStr);


						SE_CORE_ERROR(class_name + ":" +method_name + " " + error + "\n");
					}

					return returnvalue;
				}
			}
		}
	}

	//Script Instance

	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Object object)
		: m_ScriptClass(scriptClass)
	{
		m_Instance = scriptClass->Instantiate();

		m_Constructor = s_ScriptData->ObjectClass.GetMethod(".ctor", 1);
		m_OnCreateMethod = scriptClass->GetMethod("OnStart", 0);
		m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

		// Call Entity constructor
		{
			UUID objectId = object.GetComponent<TagComponent>().uuid;
			void* param = &objectId;
			m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
		}
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (m_OnCreateMethod)
			m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		if (m_OnUpdateMethod)
		{
			void* param = &ts;
			m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
		}
	}
}