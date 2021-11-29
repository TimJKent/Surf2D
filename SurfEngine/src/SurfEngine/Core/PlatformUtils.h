#pragma once

#include <string>
#include <optional>

namespace SurfEngine {

	class FileDialogs
	{
	public:
		static std::string OpenFile(const std::string& start_path, const char* filter);
		static std::string SaveFile(const char* filter);
	};

}

