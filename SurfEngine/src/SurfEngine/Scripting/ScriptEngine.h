#pragma once
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "SurfEngine/Scenes/Scene.h"

#include <filesystem>

namespace SurfEngine {

	struct ScriptEngineStorage {
		MonoDomain* Root_Domain = nullptr;
		MonoDomain* App_Domain = nullptr;

		MonoAssembly* Core_Assembly = nullptr;
		MonoImage* Core_Assembly_Image = nullptr;

		std::string csc_path = "C:\\Windows\\Microsoft.NET\\Framework64\\v4.0.30319\\csc.exe";
	};

	class ScriptEngine
	{
	public:
		static void Init();
		static void SceneStart();
		static void SceneEnd();
		static MonoImage* GetImage();
		static void SetCurrentScene(Scene* scene);
		static MonoString* CreateMonoString(const std::string& string);
	public:
		static ScriptEngineStorage* s_Data;
	private:
		static void LoadAssembly(std::filesystem::path path);
	};

	class ScriptClass {
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& class_namespace, const std::string& class_name);

		MonoObject* CreateInstance();

		MonoMethod* GetMethod(const std::string& name, std::size_t parameter_count);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);
		MonoClass* monoclass;
	private:
	};
}


