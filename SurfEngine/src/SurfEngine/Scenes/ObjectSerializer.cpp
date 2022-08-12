#include "sepch.h"
#include "ObjectSerializer.h"
#include "SceneSerializer.h"
#include "Components.h"
#include <fstream>
#include <yaml-cpp/yaml.h>


namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
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

	template<>
	struct convert<SurfEngine::Script_Var> {

		static bool decode(const Node& node, SurfEngine::Script_Var& sv)
		{
			if (!node.IsSequence() || node.size() != 5)
				return false;

			sv.name = node[0].as<std::string>();
			sv.type = node[1].as<std::string>();
			sv.default_value = node[2].as<std::string>();
			sv.isUserValueDefined = node[3].as<bool>();
			sv.user_value = node[4].as<std::string>();
			return true;
		}
	};
}

namespace SurfEngine {

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
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
	
	YAML::Emitter& operator<<(YAML::Emitter& out, const Script_Var& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.name << v.type << v.default_value << v.isUserValueDefined << v.user_value << YAML::EndSeq;
		return out;
	}

	void ObjectSerializer::SerializeObject(YAML::Emitter& out, Object object)
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
			{
				parent = (uint64_t)tc.parent->gameObject.GetComponent<TagComponent>().uuid;
			}
			out << YAML::Key << "Parent" << YAML::Value << parent;
			out << YAML::Key << "ChildCount" << YAML::Value << tc.children.size();

			for (int i = 0; i < tc.children.size(); i++)
			{
				uint64_t child = 0;
				child = (uint64_t)tc.children[i]->gameObject.GetComponent<TagComponent>().uuid;
				out << YAML::Key << "Child" + std::to_string(i) << YAML::Value << child;
			}

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
			out << YAML::Key << "Scaling" << YAML::Value << spriteRendererComponent.scaling;
			out << YAML::Key << "Offset" << YAML::Value << spriteRendererComponent.offset;

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

		if (object.HasComponent<ScriptComponent>())
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap; // Script Component

			auto& scriptComponent = object.GetComponent<ScriptComponent>();

			out << YAML::Key << "Path" << YAML::Value << scriptComponent.path;
			out << YAML::Key << "NumVar" << YAML::Value << scriptComponent.variables.size();
			for (int i = 0; i < scriptComponent.variables.size(); i++) {
				std::string name = "var" + std::to_string(i);
				out << YAML::Key << name << YAML::Value << scriptComponent.variables[i];
			}
			out << YAML::EndMap; 
		}

		if (object.HasComponent<RigidbodyComponent>())
		{
			RigidbodyComponent rbc = object.GetComponent<RigidbodyComponent>();

			out << YAML::Key << "RigidBodyComponent";
			out << YAML::BeginMap;
			
			
			out << YAML::Key << "BodyType" << YAML::Value << (int)rbc.Type;
			out << YAML::Key << "FixedRotation" << YAML::Value << rbc.FixedRotation;

			out << YAML::EndMap;
		}

		if (object.HasComponent<BoxColliderComponent>())
		{
			BoxColliderComponent bc = object.GetComponent<BoxColliderComponent>();

			out << YAML::Key << "BoxColliderComponent";
			out << YAML::BeginMap;

			out << YAML::Key << "Size" << YAML::Value << bc.Size;
			out << YAML::Key << "Offset" << YAML::Value << bc.Offset;
			out << YAML::Key << "PhysicsMaterialPath" << YAML::Value << bc.physics_material_path;

			out << YAML::EndMap;
		}

		if (object.HasComponent<CircleColliderComponent>())
		{
			CircleColliderComponent cc = object.GetComponent<CircleColliderComponent>();

			out << YAML::Key << "CircleColliderComponent";
			out << YAML::BeginMap;

			out << YAML::Key << "Radius" << YAML::Value << cc.Radius;
			out << YAML::Key << "Offset" << YAML::Value << cc.Offset;
			out << YAML::Key << "PhysicsMaterialPath" << YAML::Value << cc.physics_material_path;

			out << YAML::EndMap;
		}

		out << YAML::EndMap; // Object
	}

	void ObjectSerializer::DeserialzeObject(YAML::Node data, Ref<Scene> scene) {

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
				

				//SE_CORE_TRACE("Deserialized object with ID = {0}, name = {1}", uuid, name);

				Object deserializedObject = scene->CreateObject(name, uuid);

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
					auto& src			= deserializedObject.AddComponent<SpriteRendererComponent>();
					src.Color			= spriteRendererComponent["Color"].as<glm::vec4>();
					src.Layer			= spriteRendererComponent["Layer"].as<unsigned int>();
					src.Texture_Path	= spriteRendererComponent["Texture_Path"].as<std::string>();
					src.flipX			= spriteRendererComponent["FlipX"].as<bool>();
					src.reflective		= spriteRendererComponent["Reflective"].as<bool>();
					src.scaling			= spriteRendererComponent["Scaling"].as<glm::vec2>();
					src.offset			= spriteRendererComponent["Offset"].as<glm::vec2>();
					if (!src.Texture_Path.empty())
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

				auto scriptComponent = object["ScriptComponent"];
				if (scriptComponent)
				{
					auto& sc = deserializedObject.AddComponent<ScriptComponent>();
					sc.path = scriptComponent["Path"].as<std::string>();
					int num_var = scriptComponent["NumVar"].as<int>();
					for (int i = 0; i < num_var; i++) {
						std::string var_name = "var" + std::to_string(i);
						Script_Var var = scriptComponent[var_name].as<Script_Var>();
						sc.variables.push_back(var);
					}
				}

				auto rigidbodyComponent = object["RigidBodyComponent"];
				if (rigidbodyComponent)
				{
					auto& rbc = deserializedObject.AddComponent<RigidbodyComponent>();
					int bodyType = rigidbodyComponent["BodyType"].as<int>();
					switch (bodyType) {
						case 0: rbc.Type = RigidbodyComponent::BodyType::Static; break;
						case 1: rbc.Type = RigidbodyComponent::BodyType::Dynamic; break;
						case 2: rbc.Type = RigidbodyComponent::BodyType::Kinematic; break;
						default: rbc.Type = RigidbodyComponent::BodyType::Static; break;
					}
					rbc.FixedRotation = rigidbodyComponent["FixedRotation"].as<bool>();
				}

				auto boxColliderComponent = object["BoxColliderComponent"];
				if (boxColliderComponent)
				{
					auto& bc = deserializedObject.AddComponent<BoxColliderComponent>();
					bc.Size = boxColliderComponent["Size"].as<glm::vec2>();
					bc.Offset = boxColliderComponent["Offset"].as<glm::vec2>();
					bc.physics_material_path = boxColliderComponent["PhysicsMaterialPath"].as<std::string>();
					if (!bc.physics_material_path.empty()) {
						//bc.physics_material = GetPhysicsMaterialFromPath(bc.physics_material_path);
					}
				}
				auto circleColliderComponent = object["CircleColliderComponent"];
				if (circleColliderComponent)
				{
					auto& cc = deserializedObject.AddComponent<CircleColliderComponent>();
					cc.Radius = circleColliderComponent["Radius"].as<float>();
					cc.Offset = circleColliderComponent["Offset"].as<glm::vec2>();
					cc.physics_material_path = circleColliderComponent["PhysicsMaterialPath"].as<std::string>();
					if (!cc.physics_material_path.empty()) {
						//cc.physics_material = GetPhysicsMaterialFromPath(cc.physics_material_path);
					}
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


					//SE_CORE_TRACE("Deserialized object with ID = {0}, name = {1}", uuid, name);

					TransformComponent* tc = &scene->GetObjectByUUID(uuid).GetComponent<TransformComponent>();

					auto transformComponent = object["TransformComponent"];
					if (transformComponent)
					{
						uint64_t parent = transformComponent["Parent"].as<uint64_t>();
						int childcount = transformComponent["ChildCount"].as<int>();
						if (parent != 0) { tc->parent = &scene->GetObjectByUUID(parent).GetComponent<TransformComponent>(); }

						for (int i = 0; i < childcount; i++) {
							std::string childstr = "Child";
							childstr += std::to_string(i);
							uint64_t child = transformComponent[childstr].as<uint64_t>();
							tc->children.push_back(&scene->GetObjectByUUID(child).GetComponent<TransformComponent>());
						}
					}
				}
			}
		}
	
	}

	void RecSerialize(YAML::Emitter& out, Object object) {
		ObjectSerializer::SerializeObject(out, object);
		TransformComponent tc = object.GetComponent<TransformComponent>();
		std::size_t numChildren = tc.children.size();
		for (int i = 0; i < numChildren; i++) {
			RecSerialize(out, tc.children[i]->gameObject);
		}
	}

	void ObjectSerializer::Serialize(const std::string& filepath, Object object) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Object" << YAML::Value << "Object";
		out << YAML::Key << "Objects" << YAML::Value << YAML::BeginSeq;
		RecSerialize(out, object);
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
		fout.close();
	}


	bool ObjectSerializer::Deserialze(const std::string& filepath, Ref<Scene> scene) {
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			return false;
		}

		if (!data["Objects"])
			return false;

		//SE_CORE_TRACE("Deserializing object '{0}'", filepath);
		ObjectSerializer::DeserialzeObject(data, scene);
		return true;
	}
}
