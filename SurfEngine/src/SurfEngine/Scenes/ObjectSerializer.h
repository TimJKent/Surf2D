#pragma once
#include "Object.h"
#include "yaml-cpp/yaml.h"

namespace YAML {
	
}
namespace SurfEngine {


	class ObjectSerializer {
	public:
		static void SerializeObject(YAML::Emitter& out, Object object);
		static void ObjectSerializer::DeserialzeObject(YAML::Node data, Ref<Scene> scene);
	public:
		ObjectSerializer() = default;

		void Serialize(const std::string& filepath, Object object);
		bool Deserialze(const std::string& filepath, Ref<Scene> scene);
	};
}
