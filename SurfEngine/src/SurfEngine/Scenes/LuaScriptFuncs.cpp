#include "sepch.h" 
#include "LuaScriptFuncs.h"
#include "SurfEngine/Renderer/Renderer2D.h"
#include "SurfEngine/Core/KeyCodes.h"
#include "Components.h"




namespace SurfEngine {

	

	void LuaScriptFuncs::AddFuncs(sol::state& lua, SurfEngine::Object* thisObj) {
		lua.open_libraries(sol::lib::base);
		
		lua["this"] = *thisObj; 

		sol::usertype<Object> object_type = lua.new_usertype<Object>("object",
			// 3 constructors
			sol::constructors<Object()>());

		sol::usertype<TransformComponent> transform_type = lua.new_usertype<TransformComponent>("transform", sol::constructors<TransformComponent()>());
		sol::usertype<SpriteRendererComponent> spriterender_type = lua.new_usertype<SpriteRendererComponent>("sprite", sol::constructors<SpriteRendererComponent()>());
		sol::usertype<CameraComponent> camera_type = lua.new_usertype<CameraComponent>("camera", sol::constructors<CameraComponent()>());
		sol::usertype<AnimationComponent> animation_type = lua.new_usertype<AnimationComponent>("animation", sol::constructors<AnimationComponent()>());
		sol::usertype<LuaScriptComponent> script_type = lua.new_usertype<LuaScriptComponent>("script", sol::constructors<LuaScriptComponent()>());



		//Log
		lua.set_function("Log", [](int arg) { SE_INFO(arg); });
		lua.set_function("Log", [](std::string arg) { SE_INFO(arg); });
		
		//Input
		lua.set_function("IsKeyPressed", [](int keycode) {return Input::IsKeyPressed(keycode); });
		lua.set_function("IsMouseButtonPressed", [](int button) {return Input::IsMouseButtonPressed(button); });
		lua.set_function("GetMouseX", []() {return Input::GetMouseX(); });
		lua.set_function("GetMouseY", []() {return Input::GetMouseY(); });

		//Util
		lua.set_function("SetResolution", [](int x, int y) {Renderer2D::SetRenderSize(x, y); });
		

		//EnTT

		lua["object"]["GetObject"] = [](Object& o, std::string uuid) {
			return o.GetScene()->GetObjectByUUID(std::stoull(uuid));
		};

		lua["object"]["GetTransform"] = [](Object& o) {
			return &o.GetComponent<TransformComponent>(); 
		};
		lua["object"]["GetSprite"] = [](Object& o) {
			return &o.GetComponent<SpriteRendererComponent>();
		};
		lua["object"]["GetCamera"] = [](Object& o) {
			return &o.GetComponent<CameraComponent>();
		}; 
		lua["object"]["GetAnimation"] = [](Object& o) {
			return &o.GetComponent<AnimationComponent>();
		};

		lua["object"]["HasComponent"] = [](Object& o, std::string compname) {

			if (compname._Equal("Transform")) { return o.HasComponent<TransformComponent>(); }
			if (compname._Equal("Animation")) { return o.HasComponent<AnimationComponent>(); }
			if (compname._Equal("SpriteRenderer")) { return o.HasComponent<SpriteRendererComponent>(); }
			if (compname._Equal("Camera")) { return o.HasComponent<CameraComponent>(); }
			if (compname._Equal("Script")) { return o.HasComponent<LuaScriptComponent>(); }
		};

		//Transform
		{
			lua["transform"]["SetPosition"] = [&](TransformComponent& o, float x, float y) {
				o.Translation.x = x;
				o.Translation.y = y;
			};

			lua["transform"]["SetPositionX"] = [&](TransformComponent& o, float x) {
				o.Translation.x = x;
			};

			lua["transform"]["SetPositionY"] = [&](TransformComponent& o, float y) {
				o.Translation.y = y;
			};

			lua["transform"]["SetRotation"] = [&](TransformComponent& o, float y) {
				o.Rotation.y = y;
			};

			lua["transform"]["GetPositionX"] = [](TransformComponent& o, std::string compname) {
				return	o.Translation.x;
			};

			lua["transform"]["GetPositionY"] = [](TransformComponent& o, std::string compname) {
				return	o.Translation.y;
			};

			lua["transform"]["Rotate"] = [&](TransformComponent& o, float amount) {
				o.Rotation.z += amount;
			};

			lua["transform"]["TranslateX"] = [&](TransformComponent& o, float amount) {
				o.Translation.x += amount;
			};

			lua["transform"]["TranslateY"] = [&](TransformComponent& o, float amount) {
				o.Translation.y -= amount;
			};
		}

		//SpriteRenderer
		{
			lua["sprite"]["GetColor"] = [&](SpriteRendererComponent& o) {
				return std::make_tuple(o.Color.r, o.Color.g, o.Color.b, o.Color.a);
			};

			lua["sprite"]["SetColorRGB"] = [&](SpriteRendererComponent& o, float r, float g, float b) {
				o.Color = glm::vec4(r, g, b,1.f);
			};

			lua["sprite"]["SetColorRGBA"] = [&](SpriteRendererComponent& o, float r, float g, float b, float a) {
				o.Color = glm::vec4(r, g, b, a);
			};

			lua["sprite"]["SetSprite"] = [&](SpriteRendererComponent& o, std::string path) {
					o.Texture = Texture2D::Create(path);
					o.Texture_Path = path;
			};

			lua["sprite"]["IsFlippedX"] = [&](SpriteRendererComponent& o) {
				return o.flipX;
			};

			lua["sprite"]["SetFlippedX"] = [&](SpriteRendererComponent& o, bool flip) {
				o.flipX = flip;
			};

			lua["sprite"]["GetLayer"] = [&](SpriteRendererComponent& o) {
				return o.Layer;
			};

			lua["KeyCode"] = [&](SpriteRendererComponent& o, int layer) {
				o.Layer = layer;
			};
		}
		
		//Camera
		{
			lua["camera"]["GetSize"] = [&](CameraComponent& o) {
				return o.Camera.GetOrthographicSize();
			};
			lua["camera"]["GetNearClip"] = [&](CameraComponent& o) {
				return o.Camera.GetOrthographicNearClip();
			};
			lua["camera"]["GetFarClip"] = [&](CameraComponent& o) {
				return o.Camera.GetOrthographicFarClip();
			};

			lua["camera"]["SetSize"] = [&](CameraComponent& o, float size) {
				return o.Camera.SetOrthographicSize(size);
			};
			lua["camera"]["SetNearClip"] = [&](CameraComponent& o, float dist) {
				return o.Camera.SetOrthographicNearClip(dist);
			};
			lua["camera"]["SetFarClip"] = [&](CameraComponent& o, float dist) {
				return o.Camera.SetOrthographicFarClip(dist);
			};
		}
		
		//Animation
		{
			lua["animation"]["GetCurrentFrame"] = [&](AnimationComponent& o) {
				return o.currframe;
			};
			lua["animation"]["SetCurrentFrame"] = [&](AnimationComponent& o, int frame) {
				o.currframe = frame;
			};

			lua["animation"]["GetTotalFrames"] = [&](AnimationComponent& o) {
				return o.frames;
			};
			lua["animation"]["SetTotalFrames"] = [&](AnimationComponent& o, int totalFrames) {
				o.frames = totalFrames;
			};

			lua["animation"]["GetFps"] = [&](AnimationComponent& o) {
				return o.fps;
			};
			lua["animation"]["SetFps"] = [&](AnimationComponent& o, int fps) {
				o.fps = fps;
			};

			lua["animation"]["IsLooping"] = [&](AnimationComponent& o) {
				return o.loop;
			};
			lua["animation"]["SetLooping"] = [&](AnimationComponent& o, bool loop) {
				o.loop = loop;
			};

			lua["animation"]["IsPlayOnAwake"] = [&](AnimationComponent& o) {
				return o.playOnAwake;
			};
			lua["animation"]["SetPlayOnAwake"] = [&](AnimationComponent& o, bool playOnAwake) {
				o.playOnAwake = playOnAwake;
			};

			lua["animation"]["Play"] = [&](AnimationComponent& o) {
				o.play = true;
			};

			lua["animation"]["Stop"] = [&](AnimationComponent& o) {
				o.play = false;
			};

			lua["animation"]["StopAndReset"] = [&](AnimationComponent& o) {
				o.play = false;
				o.currframe = 0;
			};

			lua.set_function("KeyCode", [](std::string arg) {
				if (arg._Equal("A")) { return SE_KEY_A; }
				if (arg._Equal("B")) { return SE_KEY_B; }
				if (arg._Equal("C")) { return SE_KEY_C; }
				if (arg._Equal("D")) { return SE_KEY_D; }
				if (arg._Equal("E")) { return SE_KEY_E; }
				if (arg._Equal("F")) { return SE_KEY_F; }
				if (arg._Equal("G")) { return SE_KEY_G; }
				if (arg._Equal("H")) { return SE_KEY_H; }
				if (arg._Equal("I")) { return SE_KEY_I; }
				if (arg._Equal("J")) { return SE_KEY_J; }
				if (arg._Equal("K")) { return SE_KEY_K; }
				if (arg._Equal("L")) { return SE_KEY_L; }
				if (arg._Equal("M")) { return SE_KEY_M; }
				if (arg._Equal("N")) { return SE_KEY_N; }
				if (arg._Equal("O")) { return SE_KEY_O; }
				if (arg._Equal("P")) { return SE_KEY_P; }
				if (arg._Equal("Q")) { return SE_KEY_Q; }
				if (arg._Equal("R")) { return SE_KEY_R; }
				if (arg._Equal("S")) { return SE_KEY_S; }
				if (arg._Equal("T")) { return SE_KEY_T; }
				if (arg._Equal("U")) { return SE_KEY_U; }
				if (arg._Equal("V")) { return SE_KEY_V; }
				if (arg._Equal("W")) { return SE_KEY_W; }
				if (arg._Equal("X")) { return SE_KEY_X; }
				if (arg._Equal("Y")) { return SE_KEY_Y; }
				if (arg._Equal("Z")) { return SE_KEY_Z; }
				if (arg._Equal("Space")) { return SE_KEY_SPACE; }
				if (arg._Equal("LftShift")) { return SE_KEY_LEFT_SHIFT; }
				
			});

		}
	}
}
