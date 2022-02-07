#pragma once

#include "Object.h"

namespace SurfEngine {
	class ScriptableObject {
	public:
		virtual ~ScriptableObject() {}

	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(Timestep ts) {}

	private:
		Object m_Object;
		friend class Scene;
		friend class CameraController;
	};

}