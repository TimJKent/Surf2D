#include "sepch.h"
#include "Resources.h"

namespace SurfEngine {

	std::string Resources::project_directory_path = "";

	

	std::string Resources::GetProjectDirectory() {
		return project_directory_path;
	}


	void Resources::SetProjectDirectory(const std::string& project_dir_file_path) {
		project_directory_path = project_dir_file_path;
	}

}