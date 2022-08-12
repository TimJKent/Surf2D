#include "ProjectManager.h"
#include <yaml-cpp/yaml.h>
#include "SurfEngine/Physics/PhysicsEngine.h"
#include "AssetSerializer.h"
#include "SceneManager.h"
#include "FileManager.h"
#include <fstream>


namespace SurfEngine {

	 Ref<Project> ProjectManager::s_ActiveProject = nullptr;
	 std::string  ProjectManager::s_ProjectsDirPath = "";
	 std::string  ProjectManager::s_SelectedPath = "";
	 std::string  ProjectManager::s_RootPath = "";
	 std::string  ProjectManager::s_HighestDirectory = "";

	bool ProjectManager::IsActiveProject() { return s_ActiveProject.use_count() != 0; }

	void ProjectManager::SetSelectedPath(const std::string& path) {
		ProjectManager::s_SelectedPath = path;
		std::filesystem::directory_entry p = std::filesystem::directory_entry(path);
		if (!p.is_directory()) {
			SceneManager::ClearSelectedObject();
		}
	}

	void ProjectManager::LoadProject(const std::string& path) {
		SceneManager::ClearActiveScene();

		Ref<Project> project = std::make_shared<Project>();
		std::filesystem::path p = std::filesystem::path(path);
		project->SetName(p.stem().string());
		project->SetProjectDirectory(p.parent_path().string());

		AssetSerializer::LoadProjectProperties(path);

		s_ActiveProject = project;

		SetWindowTitle();
		ProjectManager::SetPath(ProjectManager::GetActiveProject()->GetProjectDirectory());
		ProjectManager::SetHighestPath(ProjectManager::GetActiveProject()->GetProjectDirectory());
	}

	void ProjectManager::CreateProject(const std::string& filename) {
		if (FileManager::HasDirectory(s_ProjectsDirPath, filename)) {
			SE_CORE_ERROR("Failed to Create Project with name:" + filename + "Project already exists with that name");
			return; 
		}
		if (filename.empty()) {
			SE_CORE_ERROR("Failed to Create Project (empty name)");
			return;
		}
		SceneManager::ClearActiveScene();
		Ref<Project> project = std::make_shared<Project>(filename);
		project->SetProjectDirectory(CreateProjectDirectory(filename));
		
		AssetSerializer::SaveProjectProperties(filename);

		s_ActiveProject = project;
		SetWindowTitle();
		ProjectManager::SetPath(ProjectManager::GetActiveProject()->GetProjectDirectory());
		ProjectManager::SetHighestPath(ProjectManager::GetActiveProject()->GetProjectDirectory());
	}

	void ProjectManager::InitProjectsDirectory() {
		s_ProjectsDirPath = FileManager::GetDocumentsDir();
		if (!HasProjectsDirectory(s_ProjectsDirPath)) {
			s_ProjectsDirPath = CreateProjectsDirectory(s_ProjectsDirPath);
		}
		else {
			s_ProjectsDirPath = FileManager::GetDocumentsDir() + "\\SurfEngine";
		}
		SE_CORE_INFO("Set Projects Directory to: " + s_ProjectsDirPath);
		ProjectManager::SetPath(s_ProjectsDirPath);
		ProjectManager::SetHighestPath(s_ProjectsDirPath);
	}

	

	bool ProjectManager::HasProjectsDirectory(const std::string& documentsdir) {
		return FileManager::HasDirectory(documentsdir, "SurfEngine");
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
			SE_CORE_INFO("Successfully Created Project Directory at: " + projectDirectory);
			return projectDirectory;
		}
		else {
			SE_CORE_ERROR("ERROR: Could not create Project Directory!");
		}
		return "";
	}

	Ref<Project>& ProjectManager::GetActiveProject() { return s_ActiveProject; }

	void ProjectManager::SetWindowTitle() {
		std::string title = "Surf2d - ";
		std::string project = "Please Open or Create a Project";
		std::string scene = "\\";
		if (ProjectManager::IsActiveProject()) {
			project = ProjectManager::GetActiveProject()->GetName();
		}
		if (SceneManager::IsActiveScene()) {
			scene += SceneManager::GetActiveScene()->GetName();
		}
		GLFWwindow* window = (GLFWwindow*)SurfEngine::Application::Get().GetWindow().GetNativeWindow();
		std::string window_title = title + project + scene;
		glfwSetWindowTitle(window, window_title.c_str());
	}


	void ProjectManager::SetPath(const std::string& path) { s_RootPath = path; }

	void ProjectManager::SetHighestPath(const std::string& path) { s_HighestDirectory = path; }

	const std::string& ProjectManager::GetPath() { return s_RootPath; }

	const std::string& ProjectManager::GetHighestPath() { return s_HighestDirectory; }

	void ProjectManager::CompileProjectScripts() {
		if (!IsActiveProject()) { return; }

		const std::string csc_path = ScriptEngine::s_Data->csc_path;
		const std::string flags = "/nologo /t:library /out:UserScript.dll";

		std::vector<std::string> script_filepaths;
		
		script_filepaths.push_back("/r:..\\..\\Surf2D\\bin\\Debug-windows-x86_64\\EngineEditor\\SurfLib.dll");

		entt::registry* registry = SceneManager::GetActiveScene()->GetRegistry();

		registry->view<ScriptComponent>().each([&](auto object, ScriptComponent& sc) {
			std::vector<Script_Var> variables;


			std::filesystem::path file_path = sc.path;
			if (file_path.extension().compare(".cs") == 0) {
				if (std::filesystem::exists(file_path)) {
					if (std::find(script_filepaths.begin(), script_filepaths.end(), sc.path) == script_filepaths.end()) {
						script_filepaths.push_back(sc.path);
						std::ifstream input(sc.path);

						for (std::string line; std::getline(input, line); )
						{
							if (line.find("public") != std::string::npos && line.find(";") != std::string::npos) {
								Script_Var variable;
								
								line = line.substr(line.find("public") + 7, line.find(";"));
								variable.type = line.substr(0, line.find_first_of(' '));
								line = line.substr(line.find_first_of(' ') + 1);

								variable.name = line.substr(0, line.find_first_of(' '));

								if (variable.name.find(';') != std::string::npos) {
									variable.name = variable.name.substr(0, variable.name.length() - 1);
								}

								line = line.substr(line.find_first_of(' ') + 1);
								variable.default_value = "";

								if (line.find("=") != std::string::npos) {
									variable.default_value = line.substr(line.find_first_of(' ') + 1, line.find_last_of(';') - 2);
								}
								
								if (!variable.isUserValueDefined) {
									variable.user_value = variable.default_value;
								}

								for (int i = 0; i < sc.variables.size(); i++) {
									if ((sc.variables[i].name == variable.name) && (sc.variables[i].type == variable.type) && (sc.variables[i].isUserValueDefined)) {
										variable.isUserValueDefined = true;
										variable.user_value = sc.variables[i].user_value;
										break;
									}
								}
								variables.push_back(variable);
							}
						}
						sc.variables = variables;
					}
				}
				else {
					SE_CORE_WARN("SCRIPT_ENGINE: Script specified at [%s] does not exist! !", sc.path);
				}
			}
			else {
				SE_CORE_WARN("SCRIPT_ENGINE: Invalid Script Type![%s]", file_path.extension().string());
			}
		});

		std::string final_paths = "";
		for (std::string p : script_filepaths) {
			final_paths += " ";
			final_paths += p;
		}
		
		const std::string compile_cmd = csc_path + " " + flags + final_paths;

		system(compile_cmd.c_str());

		SE_CORE_TRACE("Moving UserScript.dll");
		const std::string dll_src = "UserScript.dll";
		const std::string dll_dest = "..\\..\\Surf2D\\bin\\Debug-windows-x86_64\\EngineEditor\\UserScript.dll";
		const std::string move_cmd = "MOVE " + dll_src + " " + dll_dest;

		system(move_cmd.c_str());
	}

	void ProjectManager::CompileRootLib() {
		const std::string csc_path = "C:\\Windows\\Microsoft.NET\\Framework64\\v4.0.30319\\csc.exe";
		const std::string flags = "/nologo /t:library /out:SurfLib.dll ..\\..\\Surf2D\\SurfEngine\\SurfLib\\Libs.cs";
		const std::string compile_cmd = csc_path + " " + flags;

		system(compile_cmd.c_str());

		SE_CORE_TRACE("Moving SurfLib.dll");
		const std::string dll_src = "SurfLib.dll";
		const std::string dll_dest = "..\\..\\Surf2D\\bin\\Debug-windows-x86_64\\EngineEditor\\SurfLib.dll";
		const std::string move_cmd = "MOVE " + dll_src + " " + dll_dest;

		system(move_cmd.c_str());
	}

	void ProjectManager::OpenFileInEditor(const std::filesystem::directory_entry& path) {
		if (path.path().extension().string().compare(".scene") == 0) {
			SceneManager::LoadScene(path.path().string());
		}
		else if (path.path().extension().string().compare(".surf") == 0) {
			ProjectManager::LoadProject(path.path().string());
		}
		else {
			system(path.path().string().c_str());
		}
	}
}
