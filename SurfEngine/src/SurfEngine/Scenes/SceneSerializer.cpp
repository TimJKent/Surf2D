#include "sepch.h"
#include "SceneSerializer.h"

#include "Object.h"
#include "Components.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace YAML {
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace SurfEngine {
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
	:m_Scene(scene)
	{
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static void SerializeObject(YAML::Emitter& out, Object object)
	{
		out << YAML::BeginMap; // Object

		if (object.HasComponent<TagComponent>())
		{
			TagComponent& tag = object.GetComponent<TagComponent>();
			out << YAML::Key << "Object" << YAML::Value << tag.uuid; // TODO: Object ID goes here
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			out << YAML::Key << "Tag" << YAML::Value << tag.Tag;

			out << YAML::EndMap; // TagComponent
		}

		if (object.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& tc = object.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap; // TransformComponent
		}


		if (object.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto& spriteRendererComponent = object.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		out << YAML::EndMap; // Object
	}

	void SceneSerializer::Serialze(const std::string& filepath){
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Objects" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto objectId)
			{
				Object object = { objectId, m_Scene.get() };
				if (!object)
					return;

				SerializeObject(out, object);
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
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
		SE_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto objects = data["Objects"];
		if (objects)
		{
			for (auto object : objects)
			{
				std::string name;
				uint64_t uuid;
			
				auto tagComponent = object["TagComponent"];
				if (tagComponent) {
					name = tagComponent["Tag"].as<std::string>();
					uuid = object["Object"].as<uint64_t>();
				}


				SE_CORE_TRACE("Deserialized object with ID = {0}, name = {1}", uuid, name);

				Object deserializedObject = m_Scene->CreateObject(name,uuid);

				auto transformComponent = object["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					auto& tc = deserializedObject.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}


				auto spriteRendererComponent = object["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedObject.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
				}
			}
		}

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