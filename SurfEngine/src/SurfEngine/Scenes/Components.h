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


		glm::vec3 GetTranslation() const {
			glm::vec3 pt = { 0,0,0 };
			if (parent) {
				 pt = parent->GetTranslation();
			}
			return glm::vec3{ Translation.x+pt.x, Translation.y + pt.y, Translation.z + pt.z };
		}

		glm::vec3 GetRotation() const {
			glm::vec3 pt = { 0,0,0 };
			if (parent) {
				pt = parent->GetRotation();
			}
			return glm::vec3{ Rotation.x + pt.x, Rotation.y + pt.y, Rotation.z + pt.z };
		}

		glm::vec3 GetScale() const {
			glm::vec3 pt = { 1,1,1 };
			if (parent) {
				pt = parent->GetScale();
			}
			return glm::vec3{ Scale.x * pt.x, Scale.y * pt.y, Scale.z * pt.z };
		}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(GetRotation()));

			return glm::translate(glm::mat4(1.0f), GetTranslation())
				* rotation
				* glm::scale(glm::mat4(1.0f), GetScale());
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



