#pragma once
#include "MechEngine/Core/Object.h"

namespace MechEngine {
	
	class Component {
	public:
		Component() {}

		virtual void DrawUI() = 0;
		virtual void OnUpdate() = 0;
	public:

	protected:
		Object* parentObject;
	};
}