#pragma once
#include "Project.h"
#include "SurfEngine/Scenes/Scene.h"

#include <glfw/include/GLFW/glfw3.h>

#include <string>
#include <filesystem>
#include <fstream>
#include <windows.h>
#include <iostream>
#include <shlobj.h>

#pragma comment(lib, "shell32.lib")

namespace SurfEngine {
		
		

	class ProjectManager {
	public:

		static std::string s_ProjectsDirPath;
		static Ref<Project> s_ActiveProject;
		static Ref<Scene> s_ActiveScene;
		static std::string s_RootPath;
		static std::string s_HighestDirectory;

		static bool IsActiveProject();
		static bool IsActiveScene();
		static void SetActiveProject(Ref<Project>& proj);
		static void SetActiveScene(Ref<Scene>& scene);
		static void OpenProject(const std::string& filename);

		static void CreateProject(const std::string& filename);

		static void OpenScene(const std::string& filename);
		static void InitProjectsDirectory();

		static std::string GetDocumentsDir();

		static bool HasProjectsDirectory(std::string documentsdir);

		static std::string CreateProjectsDirectory(std::string documentsdir);

		static std::string GetProjectsDirectory();

		static std::string CreateProjectDirectory(std::string project_name);

		static Ref<Project>& GetActiveProject();
		static Ref<Scene>& GetActiveScene();

		static void SetWindowTitle();


		static void SetPath(const std::string& path);

		static void SetHighestPath(const std::string& path);

		static const std::string& GetPath();

		static const std::string& GetHighestPath();
	};
}