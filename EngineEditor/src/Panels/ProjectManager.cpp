#include "ProjectManager.h"

namespace SurfEngine {

	 Ref<Project> ProjectManager::s_ActiveProject;
	 Ref<Scene>   ProjectManager::s_ActiveScene;
	 std::string  ProjectManager::s_ProjectsDirPath = "";
	 std::string  ProjectManager::s_RootPath = "";
	 std::string  ProjectManager::s_HighestDirectory = "";

	 bool ProjectManager::IsActiveProject() { return s_ActiveProject.use_count() != 0; }
	 bool ProjectManager::IsActiveScene() { return s_ActiveScene.use_count() != 0; }

	 void ProjectManager::SetActiveProject(Ref<Project>& proj) { s_ActiveProject = proj; SetWindowTitle(); }
	 void ProjectManager::SetActiveScene(Ref<Scene>& scene) { s_ActiveScene = scene; SetWindowTitle(); }

	void ProjectManager::OpenProject(const std::string& filename) {
		std::filesystem::path p = std::filesystem::path(filename);
		p = p.parent_path();
		Ref<Project> project = std::make_shared<Project>();
		project->SetName(p.filename().string());
		project->SetProjectDirectory(p.string());
		ProjectManager::SetActiveProject(project);
		SetPath(p.string());
		SetHighestPath(p.string());
	}

	void ProjectManager::CreateProject(const std::string& filename) {
		Ref<Project> project = std::make_shared<Project>(filename);
		project->SetProjectDirectory(CreateProjectDirectory(filename));
		std::fstream file;
		file.open(project->GetProjectDirectory() + "\\" + filename + ".surf", std::fstream::out);
		file << "ProjectName: " << filename;
		file.close();
		ProjectManager::SetActiveProject(project);
		ProjectManager::SetPath(ProjectManager::GetActiveProject()->GetProjectDirectory());
		ProjectManager::SetHighestPath(ProjectManager::GetActiveProject()->GetProjectDirectory());
	}

	void ProjectManager::OpenScene(const std::string& filename) {
		Ref<Scene> openedScene = std::make_shared<Scene>();
		SceneSerializer serializer(openedScene);
		serializer.Deserialze(filename);
		ProjectManager::SetActiveScene(openedScene);
	}

	void ProjectManager::InitProjectsDirectory() {
		s_ProjectsDirPath = GetDocumentsDir();
		if (!HasProjectsDirectory(s_ProjectsDirPath)) {
			s_ProjectsDirPath = CreateProjectsDirectory(s_ProjectsDirPath);
		}
		else {
			s_ProjectsDirPath = GetDocumentsDir() + "\\SurfEngine\\";
		}
		ProjectManager::SetPath(s_ProjectsDirPath);
		ProjectManager::SetHighestPath(s_ProjectsDirPath);
	}

	std::string ProjectManager::GetDocumentsDir() {
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

	bool ProjectManager::HasProjectsDirectory(std::string documentsdir) {
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

	std::string ProjectManager::CreateProjectsDirectory(std::string documentsdir) {
		SE_CORE_WARN("Warning: Projects Directory not found. Creating new Projects Directory...");
		std::string projectsDirectory = documentsdir + "\\SurfEngine";
		std::filesystem::create_directory(projectsDirectory);
		if (std::filesystem::is_directory(projectsDirectory)) {
			SE_CORE_INFO("Successfully Created Projects Directory");
			s_ProjectsDirPath = projectsDirectory;
			return projectsDirectory;
		}
		else {
			SE_CORE_ERROR("ERROR: Could not create Projects Directory!");
		}
		return "";
	}

	std::string ProjectManager::GetProjectsDirectory() {
		return s_ProjectsDirPath;
	}

	std::string ProjectManager::CreateProjectDirectory(std::string project_name) {
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

	Ref<Project>& ProjectManager::GetActiveProject() { return s_ActiveProject; }
	Ref<Scene>& ProjectManager::GetActiveScene() { return s_ActiveScene; }

	void ProjectManager::SetWindowTitle() {
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


	void ProjectManager::SetPath(const std::string& path) { s_RootPath = path; }

	void ProjectManager::SetHighestPath(const std::string& path) { s_HighestDirectory = path; }

	const std::string& ProjectManager::GetPath() { return s_RootPath; }

	const std::string& ProjectManager::GetHighestPath() { return s_HighestDirectory; }
}
