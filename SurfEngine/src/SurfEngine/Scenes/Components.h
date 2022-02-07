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


#define SOL_SAFE_FUNCTION 1
#include "sol/sol.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>



namespace SurfEngine{

	enum VARTYPE {
		BOOL, INT, FLOAT, STRING, UNKNOWN
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

		void TranslateX(float amount) { Translation.x += amount; }
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

		std::string script_path = "";
		sol::state lua;
		sol::protected_function luaFunc_OnStart;
		sol::protected_function luaFunc_OnUpdate;
		sol::protected_function luaFunc_OnDestroy;
		Object* thisObj;
		std::vector<Script_Var> variables;

		void OnCreate(Object* object) {
			thisObj = (Object*)object;
			lua.open_libraries(sol::lib::base);
			lua.set_function("Log", [](int arg) { SE_INFO(arg); });
			lua.set_function("Log", [](std::string arg) { SE_INFO(arg); });
			lua.set_function("IsKeyPressed", [](int keycode) {return Input::IsKeyPressed(keycode); });
			lua.set_function("IsMouseButtonPressed", [](int button) {return Input::IsMouseButtonPressed(button); });
			lua.set_function("GetObject", [&]() {return thisObj->HasComponent<CameraComponent>(); });
			lua.set_function("Translate", [&](float amount) { thisObj->GetComponent<TransformComponent>().Translation.x += amount; });
			lua.set_function("FlipSprite", [&](bool flip) { thisObj->GetComponent<SpriteRendererComponent>().flipX = flip; });
			lua.set_function("AnimPlay", [&](bool play) { thisObj->GetComponent<AnimationComponent>().play = play; });
			lua.set_function("SetSprite", [&](std::string loc) { thisObj->GetComponent<SpriteRendererComponent>().Texture = Texture2D::Create(loc); });
			lua.set_function("SetFrames", [&](int amount) { thisObj->GetComponent<AnimationComponent>().frames = amount; });
			lua.set_function("SetFrame", [&](int amount) { thisObj->GetComponent<AnimationComponent>().currframe = amount; });
			lua.set_function("GetMouseX", []() {return Input::GetMouseX(); });
			lua.set_function("GetMouseY", []() {return Input::GetMouseY(); });

			auto result = lua.safe_script_file(
				script_path, sol::script_pass_on_error);
			if (!result.valid()) {
				sol::error err = result;
				SE_CORE_ERROR(err.what());
			}

			luaFunc_OnStart = lua["OnStart"];
			luaFunc_OnUpdate = lua["OnUpdate"];
			luaFunc_OnDestroy = lua["OnEnd"];

			sol::protected_function_result r = luaFunc_OnStart.call();
			if(!r.valid()){
				sol::error err = r;
				std::string what = err.what();
				SE_CORE_ERROR("OnStart Error" + what);
			}
		}

		void OnUpdate(Timestep ts, Object* object) {
			thisObj = (Object*)object;
			sol::protected_function_result r = luaFunc_OnUpdate.call((float)ts);
			if (!r.valid()) {
				sol::error err = r;
				std::string what = err.what();
				SE_CORE_ERROR("OnUpdate Error" + what);
			}
		}

		void OnDestroy() {
			sol::protected_function_result r = luaFunc_OnDestroy.call();
			if (!r.valid()) {
				sol::error err = r;
				std::string what = err.what();
				SE_CORE_ERROR("OnEnd Error" + what);
			}
		}

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
					variables.push_back(Script_Var{ name, GetType(value), value });
				}	
			}
		}


		void UpdateVariables() {
			for (Script_Var& v : variables) {
				switch (v.type) {
					case VARTYPE::STRING: {
						lua.set(v.name.c_str(), v.value);
					}
					case VARTYPE::BOOL: {
						lua.set(v.name.c_str(), v.value._Equal("true"));
					}
					case VARTYPE::FLOAT: {
						lua.set(v.name.c_str(), (float)std::stod(v.value));
					}
					case VARTYPE::INT: {
						lua.set(v.name.c_str(), std::stoi(v.value));
					}
				}
			}
		}

	};


	
}



