#pragma once
#include "SurfEngine.h"

#include <string>
#include <filesystem>
#include <windows.h>
#include <iostream>
#include <shlobj.h>

#pragma comment(lib, "shell32.lib")

namespace SurfEngine {

	

	static std::string s_ProjectsDirPath;

	class Project {
	public:
		static void InitProjectsDirectory() {
			s_ProjectsDirPath = GetDocumentsDir();
			if (!HasProjectsDirectory(s_ProjectsDirPath)) {
				s_ProjectsDirPath = CreateProjectsDirectory(s_ProjectsDirPath);
			}
			else {
				s_ProjectsDirPath = s_ProjectsDirPath + "\\SurfEngine";
			}
			SE_CORE_INFO("Projects Directory set to: " + s_ProjectsDirPath);
		}

		static std::string GetDocumentsDir() {
			char* docdir = getenv("USERPROFILE");
			if (docdir)
			{
				std::string path(docdir);
				path += "\\Documents";
				return path;
			}
			SE_CORE_ERROR("User directory not found");
			return "";
		}

		static bool HasProjectsDirectory(std::string documentsdir) {
			for (const auto& entry : std::filesystem::directory_iterator(documentsdir)) {
				std::filesystem::path p = entry.path();
				if (std::filesystem::is_directory(p)) {
					if (std::strcmp(p.filename().string().c_str(), "SurfEngine") == 0) {
						return true;
					}
				}
			}
			return false;
		}

		static std::string CreateProjectsDirectory(std::string documentsdir) {
			SE_CORE_WARN("Warning: Projects Directory not found. Creating new Projects Directory...");
			std::string projectsDirectory = documentsdir + "\\SurfEngine";
			std::filesystem::create_directory(projectsDirectory);
			if (std::filesystem::is_directory(projectsDirectory)) {
				SE_CORE_INFO("Successfully Created Projects Directory");
				return projectsDirectory;
			}
			else {
				SE_CORE_ERROR("ERROR: Could not create Projects Directory!");
			}
			return "";
		}

		static std::string GetProjectsDirectory() {
			return s_ProjectsDirPath;
		}
	
		static std::string CreateProjectDirectory(std::string project_name) {
			std::string projectDirectory = s_ProjectsDirPath + "\\" + project_name;
			std::filesystem::create_directory(projectDirectory);
			if (std::filesystem::is_directory(projectDirectory)) {
				SE_CORE_INFO("Successfully Created Project Directory");
				return projectDirectory;
			}
			else {
				SE_CORE_ERROR("ERROR: Could not create Project Directory!");
			}
			return "";
		}

	public:
		Project(std::string name)
			: m_Name(name)
		{
			m_ProjectDir = CreateProjectDirectory(name);
		}

		Project() {
		}

		std::string GetProjectDirectory() {
			return m_ProjectDir;
		}

		std::string GetName() {
			return m_Name;
		}

		void SetProjectDirectory(const std::string& path) {
			m_ProjectDir = path;
		}

		void SetName(const std::string& name) {
			m_Name = name;
		}

	private:
		std::string m_Name;
		std::string m_ProjectDir;
	};

}