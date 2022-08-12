#pragma once
#include "SceneManager.h"

#include "FileManager.h"


namespace SurfEngine {
	namespace SceneManager {

		static Ref<Scene> s_ActiveScene = {};
		static Ref<Object> s_SelectedObjectContext = {};
		static std::string s_CurrentScenePath = "";

		//Scene
		bool IsActiveScene() { return s_ActiveScene.use_count() != 0; }

		Ref<Scene>& GetActiveScene() { return s_ActiveScene; }

		bool IsSelectedObject() {
			if (!s_SelectedObjectContext.get()) {
				return false;
			}
			return  *s_SelectedObjectContext.get();
		}

		Ref<Object> GetSelectedObject() { return s_SelectedObjectContext; }

		void SetSelectedObject(Ref<Object> object) { s_SelectedObjectContext = object; }

		void ClearSelectedObject() {
			if (IsSelectedObject()) {
				s_SelectedObjectContext = nullptr;
			}
		}

		void ClearActiveScene() {
			Renderer2D::ClearRenderTarget();
			ClearSelectedObject();
			s_ActiveScene.reset();
		}

		void LoadScene(const std::string& filepath) {
			if (!ProjectManager::IsActiveProject()) { return; }

			ClearActiveScene();

			Ref<Scene> openedScene = std::make_shared<Scene>();
			s_CurrentScenePath = filepath;

			SceneSerializer serializer(openedScene);
			serializer.Deserialze(filepath);
			s_ActiveScene = openedScene;
			s_CurrentScenePath = filepath;

			ProjectManager::SetWindowTitle();
			ProjectManager::CompileProjectScripts();
		}

		void SaveCurrentScene() {
			if (!ProjectManager::IsActiveProject()) { return; }
			if (!IsActiveScene()) { return; }
			SceneSerializer serializer(GetActiveScene());
			serializer.Serialze(s_CurrentScenePath);
		}

		void CreateScene(const std::string& scene_name) {
			if (!ProjectManager::IsActiveProject()) { return; }

			if (FileManager::FileExists(ProjectManager::GetPath() + "\\" + scene_name + ".scene")) { 
				SE_CORE_WARN("Scene with name [" + scene_name+ "] already exists! Aborting Scene Creation");
				return; 
			}

			ClearActiveScene();
			Ref<Scene> newScene = std::make_shared<Scene>();
			newScene->SetName(scene_name);
			s_ActiveScene = newScene;
			s_CurrentScenePath = ProjectManager::GetPath() + "\\" + scene_name + ".scene";
			ProjectManager::SetWindowTitle();
			SaveCurrentScene();
		}
	}
}