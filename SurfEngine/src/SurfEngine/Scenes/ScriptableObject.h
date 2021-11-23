#pragma once

#include "Object.h"

namespace SurfEngine {
	class ScriptableObject {
	public:

		virtual ~ScriptableObject() {}

		template<typename T>
		T& GetComponent() {
			return m_Object.GetComponent<T>();
		}

	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(Timestep ts) {}

	private:
		Object m_Object;
		friend class Scene;
	};

}