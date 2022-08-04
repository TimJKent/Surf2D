#pragma once
#include "entt.hpp"

#include "SurfEngine/Core/UUID.h"
#include "SurfEngine/Core/Timestep.h"
#include "SurfEngine/Scenes/SceneCamera.h"
#include "SurfEngine/Renderer/Camera.h"

namespace SurfEngine {
	class Object;

	class Scene {
	public:
		Scene();
		~Scene();


		void SetName(const std::string& name) { m_name = name; }
		std::string GetName() { return m_name; }

		Object CreateObject(const std::string& name = "");
		Object CreateObject(const std::string& name, UUID uuid);

		Object GetObjectByUUID(UUID uuid);
		Object GetObjectByName(std::string name);
		
		Object DuplicateObject(Object source_obj);
		void DeleteObject(Object obj);

		std::size_t ObjectCount();
		
		void OnSceneStart();
		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, Ref<SceneCamera> camera, bool draw_grid, Ref<Object> selected);
		void OnSceneEnd();


		entt::registry* GetRegistry() { return &m_Registry; }

		bool IsPlaying() { return m_IsPlaying; }

		Ref<SceneCamera> GetSceneCamera() {
			return m_sceneCamera;
		}

		void SetSceneCamera(Ref<SceneCamera> camera) {
			m_sceneCamera = camera;
		}

	private:
		bool m_IsPlaying = false;
		entt::registry m_Registry;
		std::string m_name;
		Ref<SceneCamera> m_sceneCamera;
		friend class Object;
		friend class Panel_Hierarchy;
		friend class Panel_Inspector;
		friend class SceneSerializer;
	};
}