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
#include "SurfEngine/Core/Log.h"



#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>



namespace SurfEngine{

	enum VARTYPE {
		BOOL, INT, FLOAT, STRING, OBJECT, UNKNOWN
	};

	struct Script_Var {
		std::string name;
		VARTYPE type;
		std::string value;
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
	};

	struct SpriteRendererComponent {
		glm::vec4 Color{ 1.0f,1.0f,1.0f,1.0f };
		Ref<Texture2D> Texture;
		std::string Texture_Path = "";
		bool flipX = false;
		bool reflective = false;
		unsigned int Layer = 0;
		int currFrame = 1;
		int totalFrames = 1;


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

	
	};
}



