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
		static Ref<Scene> s_ActiveScene;
		static Ref<Object> s_SelectedObjectContext;
		static std::string s_ProjectsDirPath;
		static std::string s_SelectedPath;
		static std::string s_RootPath;
		static std::string s_HighestDirectory;
		static std::string s_CurrentScenePath;

		static bool IsActiveProject();
		static bool IsActiveScene();
		static void SetActiveProject(Ref<Project>& proj);
		static void SetActiveScene(Ref<Scene>& scene);
		static void ClearActiveScene();

		static void OpenProject(const std::string& filename);

		static void CreateProject(const std::string& filename);

		static void InitProjectsDirectory();

		static std::string GetDocumentsDir();

		static bool HasProjectsDirectory(const std::string& documentsdir);

		static bool HasDirectory(const std::string& parent_path, const std::string& directory_name);

		static void CreateFileA(const std::string& parent_path, const std::string& file_name, const std::string& file_extension);

		static void CreateFileA(const std::string& path);

		static void WriteInFileA(const std::string& file_path, const std::string& msg);

		static void CreateFolder(std::string parent_path, std::string name);

		static std::string CreateProjectsDirectory(std::string documentsdir);

		static std::string GetProjectsDirectory();

		static std::string CreateProjectDirectory(std::string project_name);

		static Ref<Project>& GetActiveProject();
		static Ref<Scene>& GetActiveScene();

		static void SetWindowTitle();

		static void SetPath(const std::string& path);

		static void SetSelectedPath(const std::string& path);

		static void SetHighestPath(const std::string& path);

		static const std::string& GetPath();

		static const std::string& GetHighestPath();

		static void OpenScene(const std::string& filepath);

		static bool OpenLastScene();

		static void SaveCurrentScene();

		static void OpenFileInEditor(const std::filesystem::directory_entry& path);

		static void DeleteFileA(const std::filesystem::directory_entry& path);

		static void DuplicateFile(const std::filesystem::directory_entry& path);

		static bool FileExists(const std::filesystem::directory_entry& path);
		static bool FileExists(const std::string& path);

		static void NewScene(const std::string& name);

		static  Ref<Object> GetSelectedObject();
		static void SetSelectedObject(Ref<Object> object);
		static void ClearSelectedObject();
		static  bool IsSelectedObject();
	
	};
}