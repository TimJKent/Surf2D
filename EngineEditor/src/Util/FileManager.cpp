#pragma once
#include "FileManager.h"

namespace SurfEngine { namespace FileManager {

	std::string GetDocumentsDir() {
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

	bool HasDirectory(const std::string& dir_path, const std::string& directory_name) {
		for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
			std::filesystem::path p = entry.path();
			if (std::filesystem::is_directory(p)) {
				if (std::strcmp(p.filename().string().c_str(), directory_name.c_str()) == 0) {
					return true;
				}
			}
		}
		return false;
	}

	void CreateFolder(std::string parent_path, std::string name) {
		std::string projectsDirectory = parent_path + "\\" + name;
		std::filesystem::create_directory(projectsDirectory);
	}

	void CreateFileA(const std::string& parent_path, const std::string& file_name, const std::string& file_extension) {
		std::fstream file;
		file.open(parent_path + "\\" + file_name + file_extension, std::fstream::out);
		file.close();
	}

	void CreateFileA(const std::string& path) {
		std::fstream file;
		file.open(path, std::fstream::out);
		file.close();
	}

	void WriteInFileA(const std::string& file_path, const std::string& msg) {
		std::fstream file;
		file.open(file_path, std::fstream::out);
		file << msg;
		file.close();
	}

	void DeleteFileA(const std::filesystem::directory_entry& path) {
		std::filesystem::remove(path);
	}

	void DuplicateFile(const std::filesystem::directory_entry& path) {
		std::string source = path.path().string();
		std::string dest = path.path().stem().string();
		dest += "_dup";
		dest += path.path().extension().string();
		dest = path.path().parent_path().string() + "\\" + dest;

		if (FileExists(dest)) {
			SE_CORE_WARN("File already exists");
			return;
		}
		std::filesystem::copy_file(source, dest);
	}

	bool FileExists(const std::filesystem::directory_entry& path) {
		return std::filesystem::exists(path);
	}

	bool FileExists(const std::string& path) {
		return std::filesystem::exists(path);
	}

} }
