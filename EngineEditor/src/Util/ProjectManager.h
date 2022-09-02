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

		static Ref<Project> s_ActiveProject;
		static std::string s_ProjectsDirPath;
		static std::string s_SelectedPath;
		static std::string s_RootPath;
		static std::string s_HighestDirectory;

		static bool DebugModeOn;
		static bool DrawBackgroundGridOn;

		//Projects
		static bool IsActiveProject();
		static Ref<Project>& GetActiveProject();
		static void LoadProject(const std::string& project_path);
		static void CreateProject(const std::string& project_name);
		static void SaveProject(const std::string& project_name);
		
		static void SetWindowTitle();
		static void SetPath(const std::string& path);
		static void SetSelectedPath(const std::string& path);
		static void SetHighestPath(const std::string& path);
		static const std::string& GetPath();
		static const std::string& GetHighestPath();
		static bool HasProjectsDirectory(const std::string& documentsdir);

		static void OpenFileInEditor(const std::filesystem::directory_entry& path);

		static void InitProjectsDirectory();
		static std::string CreateProjectsDirectory(std::string documentsdir);
		static std::string CreateProjectDirectory(std::string project_name);
		static std::string GetProjectsDirectory();
		
		static void CompileProjectScripts();
		static void CompileRootLib();
	};
}