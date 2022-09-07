#pragma once
#include "SurfEngine.h"

namespace SurfEngine {

	class Resources
	{
	public:

		template<typename T>
		struct ResourcePathPair{
			std::string path;
			Ref<T> Resource;
		};

		static std::string project_directory_path;


		template<typename T>
		static ResourcePathPair<T> LoadFromResourceDir(const std::string& file_path) {
			std::string full_path = project_directory_path + "\\Resources\\" + file_path;
			ResourcePathPair<T> path_pair = { full_path,  T::Create(full_path) };
			return path_pair;
		}

		template<typename T>
		static ResourcePathPair<T> LoadFromPath(const std::string& file_path) {
			ResourcePathPair<T> path_pair = { file_path,  T::Create(file_path) };
			return path_pair;
		}

		static std::string GetProjectDirectory();
		static void SetProjectDirectory(const std::string& project_dir_file_path);
	};
}