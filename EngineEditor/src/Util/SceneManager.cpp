#pragma once
#include "SceneManager.h"
#include "ProjectManager.h"
#include "FileManager.h"
#include "SurfEngine/Scenes/Scene.h"


namespace SurfEngine {
	namespace SceneManager {

		static Ref<Scene> s_ActiveScene = {};
		static std::vector<Ref<Object>> s_SelectedObjectContext = {};
		static std::string s_CurrentScenePath = "";

		//Scene
		bool IsActiveScene() { return s_ActiveScene.use_count() != 0; }

		Ref<Scene>& GetActiveScene() { return s_ActiveScene; }

		bool IsSelectedObject() {
			if (!s_SelectedObjectContext.size()>0) {
				return false;
			}
			return  s_SelectedObjectContext.size()>0;
		}

		std::vector<Ref<Object>>& GetSelectedObject() { return s_SelectedObjectContext; }

		void PushObjectToSelection(Ref<Object> object) { s_SelectedObjectContext.push_back(object); }

		void RemoveObjectFromSelection(Ref<Object> object) {
			for (int i = 0; i < GetSelectedObjectCount(); i++) {
				if (*s_SelectedObjectContext[i].get() == *object.get())
				{
					s_SelectedObjectContext.erase(s_SelectedObjectContext.begin() + i);
					return;
				}
			}
		}

		void ClearSelectedObject() {
			s_SelectedObjectContext.clear();
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

		const std::string& GetCurrentScenePath() {
			return s_CurrentScenePath;
		}

		std::size_t GetSelectedObjectCount() {
			return s_SelectedObjectContext.size();
		}

		bool IsObjectSelected(Ref<Object> object) {
			for (int i = 0; i < s_SelectedObjectContext.size(); i++) {
				if (*s_SelectedObjectContext[i].get() == *object.get())
					return true;
			}
			return false;
		}

		void PushAllObjectsToSelection() {
			GetActiveScene()->GetRegistry()->view<TagComponent>().each([=](auto object, TagComponent& tag) {
				PushObjectToSelection(std::make_shared<Object>(object, GetActiveScene().get()));
			});
		}

		void DuplicateSelected() {
			for (int i = 0; i < s_SelectedObjectContext.size(); i++) {
				GetActiveScene()->DuplicateObject(*s_SelectedObjectContext[i].get());
			}
		}

		void DeleteSelected() {
			for (int i = 0; i < s_SelectedObjectContext.size(); i++) {
				GetActiveScene()->DeleteObject(*s_SelectedObjectContext[i].get());
			}
			ClearSelectedObject();
		}
	}
}