#pragma once
#include "entt.hpp"

#include "MechEngine/Core/Timestep.h"

namespace MechEngine {
	class Object;

	class Scene {
	public:
		Scene();
		~Scene();

		Object CreateObject(const std::string& name = "");
		void DeleteObject(Object o);
		
		unsigned int ObjectCount();

		void OnUpdate(Timestep ts);

	private:
		entt::registry m_Registry;
		
		friend class Object;
		friend class Panel_Hierarchy;
		friend class Panel_Inspector;
		friend class SceneSerializer;
	};
}