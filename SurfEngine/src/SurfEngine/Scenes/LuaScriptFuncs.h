#pragma once
#include "sepch.h"
#define SOL_SAFE_FUNCTION 1
#include "sol/sol.hpp"

#include "SurfEngine/Scenes/Object.h"
#include "SurfEngine/Scenes/Scene.h"

namespace SurfEngine {

	class LuaScriptFuncs {
	public:
		static void AddFuncs(sol::state& lua, SurfEngine::Object* thisObj);
	};
}