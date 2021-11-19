#pragma once
#include "Project.h"
#include "SurfEngine/Scenes/Scene.h"

#include <glfw/include/GLFW/glfw3.h>

namespace SurfEngine {
		static Ref<Project> s_ActiveProject;
		static Ref<Scene> s_ActiveScene;

	class ProjectManager {
	public:

		static bool IsActiveProject() { return s_ActiveProject.use_count() != 0;}
		static bool IsActiveScene() {return s_ActiveScene.use_count() != 0; }

		static void SetActiveProject(Ref<Project>& proj) { s_ActiveProject = proj; SetWindowTitle(); }
		static void SetActiveScene(Ref<Scene>& scene) { s_ActiveScene = scene; SetWindowTitle();}

		static Ref<Project>& GetActiveProject() { return s_ActiveProject; }
		static Ref<Scene>& GetActiveScene() { return s_ActiveScene; }

		static void SetWindowTitle() {
			std::string title = "Surf2d - ";
			std::string project = "Please Open or Create a Project";
			std::string scene = "\\";
			if (ProjectManager::IsActiveProject()) {
				project = ProjectManager::GetActiveProject()->GetName();
			}
			if (ProjectManager::IsActiveScene()) {
				scene += ProjectManager::GetActiveScene()->GetName(); 
			}
			GLFWwindow* window = (GLFWwindow*)SurfEngine::Application::Get().GetWindow().GetNativeWindow();
			std::string window_title = title + project + scene;
			glfwSetWindowTitle(window, window_title.c_str());
		}

	private:
	};

}