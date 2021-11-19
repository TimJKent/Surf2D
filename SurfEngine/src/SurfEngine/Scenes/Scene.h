#pragma once
#include "entt.hpp"

#include "SurfEngine/Core/UUID.h"
#include "SurfEngine/Core/Timestep.h"

namespace SurfEngine {
	class Object;

	class Scene {
	public:
		Scene();
		~Scene();

		Object CreateObject(const std::string& name = "");
		Object CreateObject(const std::string& name, UUID uuid);
		void DeleteObject(Object o);
		
		unsigned int ObjectCount();

		void OnUpdate(Timestep ts);

		void SetName(const std::string& name) { m_name = name; }
		std::string GetName() { return m_name; }

	private:
		entt::registry m_Registry;
		std::string m_name;

		friend class Object;
		friend class Panel_Hierarchy;
		friend class Panel_Inspector;
		friend class SceneSerializer;
	};
}