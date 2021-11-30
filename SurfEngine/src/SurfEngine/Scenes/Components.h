#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SurfEngine/Core/UUID.h"
#include "SurfEngine/Renderer/Texture.h"
#include "SurfEngine/Scenes/ScriptableObject.h"
#include "SceneCamera.h"


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace SurfEngine{

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
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}	
	};

	struct SpriteRendererComponent {
		glm::vec4 Color{1.0f,1.0f,1.0f,1.0f};
		Ref<Texture2D> Texture;
		std::string Texture_Path = "";
		unsigned int Layer = 0;

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

	struct NativeScriptComponent {
		ScriptableObject* Instance = nullptr;

		ScriptableObject* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind() {
			InstantiateScript = []() {return static_cast<ScriptableObject*>(new T()); }
			DestroyScript = [](NativeScriptComponent* nsc) {delete nsc->Instance; Instance = nullptr; }
		}
	};

}