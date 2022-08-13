#pragma once
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "SurfEngine/Physics/PhysicsEngine.h"
#include "ProjectManager.h"

namespace SurfEngine {
	namespace AssetSerializer{
		static void SaveProjectProperties(const std::string& project_name) {
			YAML::Emitter out;
			out << YAML::BeginMap;
			out << YAML::Key << "Project" << YAML::BeginMap;
			out << YAML::Key << "Name" << YAML::Value << project_name;
			out << YAML::Key << "Properties" << YAML::BeginMap;
			out << YAML::Key << "General" << YAML::BeginMap;
			out << YAML::EndMap;
			out << YAML::Key << "Input" << YAML::BeginMap;
			out << YAML::EndMap;
			out << YAML::Key << "Renderer" << YAML::BeginMap;
			out << YAML::EndMap;
			out << YAML::Key << "Physics" << YAML::BeginMap;
			out << YAML::Key << "gravity_scale_x" << YAML::Value << PhysicsEngine::s_Data.gravity_scale.x;
			out << YAML::Key << "gravity_scale_y" << YAML::Value << PhysicsEngine::s_Data.gravity_scale.y;
			out << YAML::Key << "velocity_iterations" << YAML::Value << PhysicsEngine::s_Data.velocity_iterations;
			out << YAML::Key << "position_iterations" << YAML::Value << PhysicsEngine::s_Data.position_iterations;
			out << YAML::EndMap;
			out << YAML::Key << "Scripting" << YAML::BeginMap;
			out << YAML::Key << "csc_path" << YAML::Value << ScriptEngine::s_Data->csc_path;
			out << YAML::EndMap;
			out << YAML::EndMap;
			out << YAML::EndMap;
			out << YAML::EndMap;

			std::ofstream file;
			file.open(ProjectManager::GetProjectsDirectory() + "\\" + project_name +"\\" + project_name + ".surf", std::fstream::out);
			file << out.c_str();
			file.close();
		}

		static void LoadProjectProperties(const std::string& file_path) {
			YAML::Node data;
			data = YAML::LoadFile(file_path); if (!data) { return; }
			auto Project = data["Project"]; if (!Project) { return; }
			auto Properties = Project["Properties"];  if (!Properties) { return; }
			auto PhysicsProperties = Properties["Physics"]; if (!PhysicsProperties) { return; }
			if (PhysicsProperties["gravity_scale_x"]) { PhysicsEngine::s_Data.gravity_scale.x = PhysicsProperties["gravity_scale_x"].as<float>(); }
			if (PhysicsProperties["gravity_scale_y"]) { PhysicsEngine::s_Data.gravity_scale.y = PhysicsProperties["gravity_scale_y"].as<float>(); }
			if (PhysicsProperties["velocity_iterations"]) { PhysicsEngine::s_Data.velocity_iterations = PhysicsProperties["velocity_iterations"].as<float>(); }
			if (PhysicsProperties["position_iterations"]) { PhysicsEngine::s_Data.position_iterations = PhysicsProperties["position_iterations"].as<float>(); }

			auto ScriptProperties = Properties["Scripting"]; if (!ScriptProperties) { return; }
			if (ScriptProperties["csc_path"]) { ScriptEngine::s_Data->csc_path = ScriptProperties["csc_path"].as<std::string>(); }

		}
	}
}