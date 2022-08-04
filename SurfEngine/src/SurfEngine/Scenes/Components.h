#pragma once
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "SurfEngine/Core/UUID.h"
#include "SurfEngine/Renderer/Texture.h"
#include "SurfEngine/Scenes/ScriptableObject.h"
#include "SurfEngine/Core/Input.h"
#include "SceneCamera.h"
#include "SurfEngine/Scripting/ScriptEngine.h"
#include "SurfEngine/Physics/PhysicsMaterial.h"


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"

namespace SurfEngine{

	struct Script_Var {
		std::string name;
		std::string type;
		std::string default_value;
		std::string user_value;

		bool isUserValueDefined = false;
	};


	struct TagComponent
	{
		std::string Tag;
		UUID uuid;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& name)
			: Tag(name), uuid() {}
		TagComponent(const std::string& name, UUID id)
			: Tag(name), uuid(id) {}

	};
	
	struct TransformComponent
	{
		Object gameObject;
		TransformComponent* parent = nullptr;
		std::vector<TransformComponent*> children = std::vector<TransformComponent*>();
	

		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(Object o) {
			gameObject = Object(o);
		}

		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}


		glm::mat4 GetTranScale() const {
			glm::mat4 parent_scale = glm::mat4(1.0f);
			if (parent) {
				parent_scale = parent->GetTranScale();
			}
			return parent_scale * glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::mat4 GetTranRot() const {
			glm::mat4 parent_transform = glm::mat4(1.0f);
			if (parent) {
				parent_transform = parent->GetTranRot();
			}

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), Translation);
			transform *= glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			return  parent_transform * transform;
		}

		glm::mat4 GetTransform() const
		{
			glm::mat4 parent_transform = glm::mat4(1.0f);
			if (parent) {
				parent_transform = parent->GetTranRot();
			}

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), Translation);
			transform *= glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0.0f,0.0f,1.0f));

			transform = parent_transform * transform;
			transform *= GetTranScale();

			return  transform ;

		}

		glm::mat4 GetTransformLocalScale() const
		{
			glm::mat4 parent_transform = glm::mat4(1.0f);
			if (parent) {
				parent_transform = parent->GetTranRot();
			}

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), Translation);
			transform *= glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

			transform = parent_transform * transform;
			transform *= glm::scale(glm::mat4(1.0f), Scale);

			return  transform;
		}

		bool HasSuperChild(TransformComponent* tc) {
			if (tc->parent) {
				if (tc->parent == this) { return true; }
				return HasSuperChild(tc->parent);
			}
			return false;
		}

		void AddChild(TransformComponent* tc) {
			if (tc) {
				children.push_back(tc);
				tc->parent = this;
			}
		}

		void RemoveChild(TransformComponent* tc) {
			for (int i = 0; i < children.size(); i++) {
				if (children[i] == tc) {
					children[i]->parent = nullptr;
					children.erase(children.begin() + i);
					break;
				}
			}
		}
	
		void SetParent(TransformComponent* tc) {
			if (tc) {
				if (HasSuperChild(tc)) {
					TransformComponent* t = tc;
					while (t->parent != this) {
						t = t->parent;
					}
					RemoveChild(t);
					t->parent = this->parent;
					if (t->parent) { t->parent->AddChild(t); }
				}
			}
			

			

			//Remove THIS from parent
			if (parent) {
				parent->RemoveChild(this);
			}

			
			parent = tc;
			if(tc)
				tc->children.push_back(this);
		}
	
	};

	struct SpriteRendererComponent {
		glm::vec4 Color{ 1.0f,1.0f,1.0f,1.0f };
		bool flipX = false;
		unsigned int Layer = 0;
		
		Ref<Texture2D> Texture;
		std::string Texture_Path = "";
		bool reflective = false;
		int currFrame = 1;
		int totalFrames = 1;
		glm::vec2 scaling = { 1.0f, 1.0f};
		glm::vec2 offset  = { 0.0f, 0.0f};


		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct CameraComponent {
		SceneCamera Camera;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct AnimationComponent {

		AnimationComponent() = default;
		AnimationComponent(const AnimationComponent&) = default;

		//Save
		int frames = 1;
		int fps = 1;
		bool playOnAwake;
		bool loop;

		//Dont Save
		bool play = true;
		int currframe = 1;
		float timer = 0.0f;
	};

	struct ScriptComponent {
		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;

		//Save
		std::string path;

		std::vector<Script_Var> variables;

		ScriptClass monoclass = {};
		MonoObject* script_class_instance = nullptr;
	};

	struct RigidbodyComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		// Storage for runtime
		void* RuntimeBody = nullptr;

		RigidbodyComponent() = default;
		RigidbodyComponent(const RigidbodyComponent&) = default;
	};


	struct BoxColliderComponent {

		glm::vec2 Size = { 1.0f, 1.0f };
		glm::vec2 Offset = { 0.0f,0.0f };

		std::string physics_material_path= "";

		PhysicsMaterial physics_material;

		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent&) = default;
	};

	struct CircleColliderComponent {

		float Radius = 0.5f;
		glm::vec2 Offset = { 0.0f,0.0f };

		std::string physics_material_path = "";
		PhysicsMaterial physics_material;

		CircleColliderComponent() = default;
		CircleColliderComponent(const CircleColliderComponent&) = default;
	};
}



