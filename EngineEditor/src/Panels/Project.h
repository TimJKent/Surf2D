#pragma once
#include "SurfEngine.h"



namespace SurfEngine {

	class Project {
	public:
		Project(std::string name)
			: m_Name(name)
		{
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