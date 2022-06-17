#include "sepch.h"
#include "SceneSerializer.h"
#include "ObjectSerializer.h"

#include "Object.h"
#include "Components.h"

#include <fstream>
#include "yaml-cpp/yaml.h"



namespace SurfEngine {
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
	:m_Scene(scene)
	{
	}

	void SceneSerializer::Serialze(const std::string& filepath){
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << m_Scene->GetName();
		out << YAML::Key << "Objects" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto objectId)
			{
				Object object = { objectId, m_Scene.get() };
				if (!object)
					return;

			ObjectSerializer::SerializeObject(out, object);
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
		fout.close();
	}

	bool SceneSerializer::Deserialze(const std::string& filepath){
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		m_Scene->SetName(sceneName);
		SE_CORE_TRACE("Deserializing scene '{0}'", sceneName);
		ObjectSerializer::DeserialzeObject(data, m_Scene);
		return true;
	}

	void SceneSerializer::SerialzeRuntime(const std::string& filepath){
		SE_CORE_ASSERT(false, "RUNTIME SERIALIZATION NOT IMPLEMENTED");
	}

	bool SceneSerializer::DeserialzeRuntime(const std::string& filepath){
		SE_CORE_ASSERT(false, "RUNTIME SERIALIZATION NOT IMPLEMENTED");
		return false;
	}
}