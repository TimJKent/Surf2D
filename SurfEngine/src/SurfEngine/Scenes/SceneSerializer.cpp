#include "sepch.h"
#include "SceneSerializer.h"

#include "Object.h"
#include "Components.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace YAML {
	
	template<>
	struct convert<SurfEngine::Script_Var> {
		static Node encode(const SurfEngine::Script_Var& var) {
			Node node;

			node.push_back(var.name);

			switch (var.type) {
			case SurfEngine::VARTYPE::STRING: {
				node.push_back("string");
				node.push_back(var.value); break;
			}
			case SurfEngine::VARTYPE::BOOL: {
				node.push_back("bool");
				node.push_back(var.value._Equal("true"));
				break;
			}
			case  SurfEngine::VARTYPE::FLOAT: {
				node.push_back("float");
				node.push_back((float)std::stod(var.value));
				break;
			}
			case SurfEngine::VARTYPE::INT: {
				node.push_back("int");
				node.push_back(std::stoi(var.value));
				break;
			}
			}
		}

		static bool decode(const Node& node, SurfEngine::Script_Var& sv)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			sv.name = node[0].as<std::string>();
			std::string type = node[1].as<std::string>();
			if (type._Equal("string")) {
				sv.type = SurfEngine::VARTYPE::STRING;
			}
			else if (type._Equal("float")) {
				sv.type = SurfEngine::VARTYPE::FLOAT;
			}
			else if (type._Equal("bool")) {
				sv.type = SurfEngine::VARTYPE::BOOL;
			}
			else if (type._Equal("int")) {
				sv.type = SurfEngine::VARTYPE::INT;
			}

			sv.value = node[2].as<std::string>();
			return true;
		}

	};

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

	YAML::Emitter& operator<<(YAML::Emitter& out, const SurfEngine::Script_Var& var)
	{
		out << YAML::Flow;
		switch (var.type) {
			case SurfEngine::VARTYPE::STRING: {
				out << YAML::BeginSeq << var.name << "string" << var.value << YAML::EndSeq;
				break;
			}
			case SurfEngine::VARTYPE::BOOL: {
				out << YAML::BeginSeq << var.name << "bool" << var.value << YAML::EndSeq;
				break;
			}
			case  SurfEngine::VARTYPE::FLOAT: {
				out << YAML::BeginSeq << var.name << "float" << var.value << YAML::EndSeq;
				break;
			}
			case SurfEngine::VARTYPE::INT: {
				out << YAML::BeginSeq << var.name << "int" << var.value << YAML::EndSeq;
				break;
			}
		}
		return out;
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
			uint64_t parent = 0;
			if (tc.parent) 
			{ parent = (uint64_t) tc.parent->gameObject.GetComponent<TagComponent>().uuid; }
			out << YAML::Key << "Parent" << YAML::Value << parent;

			uint64_t child = 0;
			if (tc.child)
			{
				child = (uint64_t)tc.child->gameObject.GetComponent<TagComponent>().uuid;
			}
			out << YAML::Key << "Child" << YAML::Value << child;

			out << YAML::EndMap; // TransformComponent
		}


		if (object.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto& spriteRendererComponent = object.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;
			out << YAML::Key << "Layer" << YAML::Value << spriteRendererComponent.Layer;
			out << YAML::Key << "Texture_Path" << YAML::Value << spriteRendererComponent.Texture_Path;
			out << YAML::Key << "FlipX" << YAML::Value << spriteRendererComponent.flipX;
			out << YAML::Key << "Reflective" << YAML::Value << spriteRendererComponent.reflective;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (object.HasComponent<AnimationComponent>())
		{
			out << YAML::Key << "AnimationComponent";
			out << YAML::BeginMap; // AnimationComponent

			auto& animationComponent = object.GetComponent<AnimationComponent>();


			out << YAML::Key << "Frames" << YAML::Value << animationComponent.frames;
			out << YAML::Key << "Fps" << YAML::Value << animationComponent.fps;
			out << YAML::Key << "PlayOnAwake" << YAML::Value << animationComponent.playOnAwake;
			out << YAML::Key << "Loop" << YAML::Value << animationComponent.loop;

			out << YAML::EndMap; // AnimationComponent
		}

		if (object.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = object.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::EndMap; // CameraComponent
		}


		out << YAML::EndMap; // Object
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

				SerializeObject(out, object);
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
					src.Layer = spriteRendererComponent["Layer"].as<unsigned int>();
					src.Texture_Path = spriteRendererComponent["Texture_Path"].as<std::string>();
					src.flipX = spriteRendererComponent["FlipX"].as<bool>();
					src.reflective = spriteRendererComponent["Reflective"].as<bool>();
					if(!src.Texture_Path.empty())
						src.Texture = Texture2D::Create(src.Texture_Path);
				}

				auto animationComponent = object["AnimationComponent"];
				if (animationComponent)
				{
					auto& ac = deserializedObject.AddComponent<AnimationComponent>();
					ac.frames = animationComponent["Frames"].as<int>();
					ac.playOnAwake = animationComponent["PlayOnAwake"].as<bool>();
					ac.loop = animationComponent["Loop"].as<bool>();
					ac.fps = animationComponent["Fps"].as<int>();
				}

				auto cameraComponent = object["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedObject.AddComponent<CameraComponent>();
					auto& cameraProps = cameraComponent["Camera"];
					cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());
				}
			}
			auto objects = data["Objects"];
			if (objects) {
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

					TransformComponent* tc = &m_Scene->GetObjectByUUID(uuid).GetComponent<TransformComponent>();

					auto transformComponent = object["TransformComponent"];
					if (transformComponent)
					{
						uint64_t parent = transformComponent["Parent"].as<uint64_t>();
						uint64_t child = transformComponent["Child"].as<uint64_t>();

						if (parent != 0) { tc->parent = &m_Scene->GetObjectByUUID(parent).GetComponent<TransformComponent>(); }
						if (child != 0) { tc->child = &m_Scene->GetObjectByUUID(child).GetComponent<TransformComponent>(); }
					}
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