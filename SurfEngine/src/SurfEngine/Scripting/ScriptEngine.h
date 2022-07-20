#pragma once
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "SurfEngine/Scenes/Scene.h"

#include <filesystem>

namespace SurfEngine {
	class ScriptEngine
	{
	public:
		static void Init();
		static MonoImage* GetImage();
		static void SetCurrentScene(Scene* scene);
		static MonoString* CreateMonoString(const std::string& string);

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


