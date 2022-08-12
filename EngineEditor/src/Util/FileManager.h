#pragma once
#include "SurfEngine.h"

namespace SurfEngine { namespace FileManager {

	bool FileExists(const std::filesystem::directory_entry& path);
	bool FileExists(const std::string& path);

	void DeleteFileA(const std::filesystem::directory_entry& path);
	void DuplicateFile(const std::filesystem::directory_entry& path);
	
	bool HasDirectory(const std::string& parent_path, const std::string& directory_name);
	std::string GetDocumentsDir();
	void CreateFileA(const std::string& parent_path, const std::string& file_name, const std::string& file_extension);
	void CreateFileA(const std::string& path);
	void WriteInFileA(const std::string& file_path, const std::string& msg);
	void CreateFolder(std::string parent_path, std::string name);
}}
