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


#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

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
		TransformComponent* child = nullptr;

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

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), GetTranslation())
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
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

	struct LuaScriptComponent {
		//Save
		std::string script_path = "";
		std::vector<Script_Var> variables;

		//DontSave
		Ref<sol::state> lua;
		Ref<sol::protected_function> luaFunc_OnStart;
		Ref<sol::protected_function> luaFunc_OnUpdate;
		Ref<sol::protected_function> luaFunc_OnEnd;

		LuaScriptComponent() = default;
		LuaScriptComponent(const LuaScriptComponent&) = default;


		static VARTYPE GetType(const std::string& str) {
			if (str.find_first_of('"') != std::string::npos) {
				return VARTYPE::STRING;
			}
			if (str._Equal("true") || str._Equal("false")) {
				return VARTYPE::BOOL;
			}
			if (str.find_first_of('.') != std::string::npos) {
				return VARTYPE::FLOAT;
			}
			return VARTYPE::INT;
		}

		void ParseScript() {
			variables.clear();
			std::string line;
			std::ifstream infile(script_path);
			while (std::getline(infile, line))
			{
				if (line._Equal("--[ShowInEditor]")) {
					std::getline(infile, line);
					std::istringstream iss(line);
					std::string name, equals, value;
					iss >> name >> equals >> value;
					if (value[0] == '"') {
						value = value.substr(1, value.length() - 1);
					}
					variables.push_back(Script_Var{ name, GetType(value), value });
				}	
			}
		}
	};


	
}



