#pragma once
#include <fstream>
#include <yaml-cpp/yaml.h>

namespace SurfEngine {

	

	static PhysicsMaterial GetPhysicsMaterialFromPath(const std::string& filepath) {
		PhysicsMaterial material;
		YAML::Node data;
		data = YAML::LoadFile(filepath);
		auto mat = data["Physics Material"];
		material.Density = mat["Density"].as<float>();
		material.Friction = mat["Friction"].as<float>();
		material.Restitution = mat["Restitution"].as<float>();
		material.RestitutionThreshold = mat["RestitutionThreshold"].as<float>();
		return material;
	}

}