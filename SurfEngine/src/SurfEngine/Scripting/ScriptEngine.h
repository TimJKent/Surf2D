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
		static MonoDomain* GetAppDomain();
		static void SetCurrentScene(Scene* scene);

	private:
			static void LoadAssembly(std::filesystem::path path);
	};
}


