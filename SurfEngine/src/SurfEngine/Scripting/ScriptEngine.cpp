#include "sepch.h"
#include "ScriptEngine.h"
#include "SurfEngine/Scripting/ScriptFuncs.h"

namespace SurfEngine {

	struct ScriptEngineStorage {
		MonoDomain* Root_Domain = nullptr;
		MonoDomain* App_Domain = nullptr;

		MonoAssembly* Core_Assembly = nullptr;
		MonoImage* Core_Assembly_Image = nullptr;
	};

	static ScriptEngineStorage* s_Data = nullptr;

	void ScriptEngine::Init() {
		SE_CORE_INFO("Starting Script Engine");
		s_Data = new ScriptEngineStorage();
		s_Data->Root_Domain = mono_jit_init("SurfMono");
		s_Data->App_Domain = mono_domain_create_appdomain("SurfDomain", NULL);
		mono_domain_set(s_Data->App_Domain, false);

		LoadAssembly("..\\..\\Surf2D\\bin\\Debug-windows-x86_64\\EngineEditor\\UserScript.dll");
	}

	void ScriptEngine::SetCurrentScene(Scene* scene) {
		current_scene = scene;
	}

	MonoString* ScriptEngine::CreateMonoString(const std::string& string) {
		return mono_string_new(s_Data->App_Domain, string.c_str());
	}

	void ScriptEngine::LoadAssembly(std::filesystem::path path) {

		s_Data->Core_Assembly = mono_domain_assembly_open(s_Data->App_Domain, path.string().c_str());

		if (!s_Data->Core_Assembly) { SE_CORE_ERROR("SCRIPT_ENGINE: Script Assembly not found!"); return; }

		InitScriptFuncs();

		s_Data->Core_Assembly_Image = mono_assembly_get_image(s_Data->Core_Assembly);

		if (!s_Data->Core_Assembly_Image) { SE_CORE_ERROR("SCRIPT_ENGINE: Failed to get Image!"); return; }

		SE_CORE_ASSERT(s_Data->Core_Assembly_Image, "Image Not Loaded!");
	}

	MonoImage* ScriptEngine::GetImage() {
		return s_Data->Core_Assembly_Image;
	}


	ScriptClass::ScriptClass(const std::string& class_namespace, const std::string& class_name) {
		monoclass = mono_class_from_name(s_Data->Core_Assembly_Image, class_namespace.c_str(), class_name.c_str());
	}

	MonoObject* ScriptClass::CreateInstance() {
		return mono_object_new(s_Data->App_Domain, monoclass);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, std::size_t parameter_count)
	{
		return mono_class_get_method_from_name(monoclass, name.c_str(), parameter_count);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		return mono_runtime_invoke(method, instance, params, nullptr);
   	}
}


