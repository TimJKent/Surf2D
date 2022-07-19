#include "sepch.h"
#include "ScriptEngine.h"

namespace SurfEngine {

	void ScriptEngine::Init() {
		mono_jit_init("SurfMono");
	}
}

