#include "sepch.h"
#include "Scene.h"

#include "Components.h"
#include "SurfEngine/Core/KeyCodes.h"
#include "SurfEngine/Renderer/Renderer2D.h"
#include "SurfEngine/Scenes/Object.h"

#include <glm/glm.hpp>


namespace SurfEngine {
	Scene::Scene(){
		m_Registry = entt::registry();
	}

	Scene::~Scene() {
		
	}

	void Scene::OnUpdateRuntime(Timestep ts) {

		m_Registry.view<CameraComponent>().each([=](auto object, CameraComponent& cc) {
			m_sceneCamera = std::make_shared<SceneCamera>(cc.Camera);
			
		});

		if (m_sceneCamera) {
			Renderer2D::BeginScene(m_sceneCamera.get());
			auto animgroup = m_Registry.group<AnimationComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : animgroup) {
				auto [anim, sprite] = animgroup.get<AnimationComponent, SpriteRendererComponent>(entity);
				if (anim.play) {
					anim.timer += ts;
				}
				if (anim.timer > (1.0f/anim.fps)) {
					anim.timer = 0.0f;
					anim.currframe++;
					if (anim.currframe > anim.frames) {
						if (!anim.loop) {
							anim.play = false;
						}
						anim.currframe = 1;
					}
				}

				sprite.currFrame = anim.currframe;
				sprite.totalFrames = anim.frames;

			}

			auto group = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
			group.sort<SpriteRendererComponent>([](const SpriteRendererComponent& lhs, const SpriteRendererComponent& rhs) {
				return lhs.Layer < rhs.Layer;
				});
			for (auto entity : group) {
				auto [sprite, transform] = group.get<SpriteRendererComponent, TransformComponent>(entity);

				if (sprite.Texture) {
					
						Renderer2D::DrawQuad(transform.GetTransform(), std::make_shared<SpriteRendererComponent>(sprite), sprite.currFrame, sprite.totalFrames);
					
				}
				else {
					Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
				}
			}
			Renderer2D::EndScene();
		}
		else {
			Renderer2D::ClearRenderTarget();
		}


		m_Registry.view<LuaScriptComponent>().each([=](auto object, LuaScriptComponent& lsc) {
			sol::protected_function_result r = lsc.luaFunc_OnUpdate->call((float)ts);
			if (!r.valid()) {
				sol::error err = r;
				std::string what = err.what();
				SE_CORE_ERROR("OnUpdate Error" + what);
			}
		});

		auto groupCamera = m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
		for (auto entity : groupCamera) {
			auto [camera, transform] = groupCamera.get<CameraComponent, TransformComponent>(entity);
			camera.Camera.m_Transform = transform.GetTransform();
			camera.Camera.UpdateView();
		}
	}


	void Scene::OnUpdateEditor(Timestep ts, Ref<SceneCamera> camera, bool draw_grid, Ref<Object> selected) {
		SetSceneCamera(camera);
		Renderer2D::BeginScene(camera.get());
		if (draw_grid) { Renderer2D::DrawBackgroundGrid(1); }
			

		auto groupCamera = m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
		
			


		auto animgroup = m_Registry.group<AnimationComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : animgroup) {
			auto [anim, sprite] = animgroup.get<AnimationComponent, SpriteRendererComponent>(entity);
			sprite.currFrame = anim.currframe;
			sprite.totalFrames = anim.frames;
		}


		

		auto group = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
		group.sort<SpriteRendererComponent>([](const SpriteRendererComponent& lhs, const SpriteRendererComponent& rhs) {
			return lhs.Layer < rhs.Layer;
			});
		for (auto entity : group) {
			auto [sprite, transform] = group.get<SpriteRendererComponent, TransformComponent>(entity);

			if (sprite.Texture) {	
				Renderer2D::DrawQuad(transform.GetTransform(), std::make_shared<SpriteRendererComponent>(sprite), sprite.currFrame, sprite.totalFrames);
			}
			else {
				Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
			}
		}

		for (auto entity : groupCamera) {
			auto [camera, transform] = groupCamera.get<CameraComponent, TransformComponent>(entity);
			glm::vec4 color = { 0.6f,0.6f,0.6f,1.0f };
			float size = camera.Camera.GetOrthographicSize();
			float height = (size / 2)*(Renderer2D::GetRenderTargetSize().x/Renderer2D::GetRenderTargetSize().y);
			Renderer2D::DrawBox({ -height,-size / 2 }, { -height,size / 2 }, { height,size / 2 }, { height,-size / 2 }, transform.GetTransform(), color);
		}

		m_Registry.view<TransformComponent>().each([=](auto object, TransformComponent& tc) {
			if (selected) {
				if (*selected.get() == object) {
					glm::vec4 color = { 1.0f,0.5f,0.0f,1.0f };
					if (!Object(object, this).HasComponent<CameraComponent>()) {
						Renderer2D::DrawBox({ -0.5f,-0.5f }, { -0.5f,0.5f }, { 0.5f,0.5f }, { 0.5f, -0.5f }, tc.GetTransform(), color);
					}
					else {
						float size = Object(object, this).GetComponent<CameraComponent>().Camera.GetOrthographicSize();
						float height = (size / 2) * (Renderer2D::GetRenderTargetSize().x / Renderer2D::GetRenderTargetSize().y);
						Renderer2D::DrawBox({ -height,-size / 2 }, { -height,size / 2 }, { height,size / 2 }, { height,-size / 2 }, tc.GetTransform(), color);
					}
				}
			}
			});
		Renderer2D::EndScene();
	}


	unsigned int Scene::ObjectCount() {
		return (int)m_Registry.size();
	}

	Object Scene::CreateObject(const std::string& name)
	{
		std::string objName = (name.empty()) ? "NewGameObject" : name;
		Object object = { m_Registry.create(), this };
		object.AddComponent<TransformComponent>();
		object.AddComponent<TagComponent>(objName);
		return object;
	}

	Object Scene::CreateObject(const std::string& name, UUID uuid)
	{
		std::string objName = (name.empty()) ? "NewGameObject" : name;
		Object object = { m_Registry.create(), this };
		object.AddComponent<TransformComponent>();
		object.AddComponent<TagComponent>(objName, uuid);
		return object;
	}

	Object Scene::DuplicateObject(entt::entity o)
	{
		Object old = Object(o, this);
		Object object = { m_Registry.create(), this };
		object.AddComponent<TransformComponent>(old.GetComponent<TransformComponent>());
		object.AddComponent<TagComponent>(old.GetComponent<TagComponent>());

		if (old.HasComponent<CameraComponent>()) { object.AddComponent<CameraComponent>(old.GetComponent<CameraComponent>()); }
		if (old.HasComponent<AnimationComponent>()) { object.AddComponent<AnimationComponent>(old.GetComponent<AnimationComponent>()); }
		if (old.HasComponent<SpriteRendererComponent>()) { object.AddComponent<SpriteRendererComponent>(old.GetComponent<SpriteRendererComponent>()); }
		if (old.HasComponent<LuaScriptComponent>()) { object.AddComponent<LuaScriptComponent>(old.GetComponent<LuaScriptComponent>()); }

		return object;
	}

	void Scene::DeleteObject(entt::entity o)
	{
		m_Registry.destroy(o);
	}



	void Scene::OnSceneEnd() {
		m_IsPlaying = false;

		m_Registry.view<LuaScriptComponent>().each([=](auto object, LuaScriptComponent& lsc) {
			sol::protected_function_result r = lsc.luaFunc_OnEnd->call();
			if (!r.valid()) {
				sol::error err = r;
				std::string what = err.what();
				SE_CORE_ERROR("OnEnd Error" + what);
			}
		});
	}

	void Scene::OnSceneStart() {
		m_IsPlaying = true;

		m_Registry.view<AnimationComponent>().each([=](auto object, AnimationComponent& ac) {
				ac.play = ac.playOnAwake;
			});

		m_Registry.view<CameraComponent>().each([=](auto object, CameraComponent& cc) {
			m_sceneCamera = std::make_shared<SceneCamera>(cc.Camera);
		});

		m_Registry.view<LuaScriptComponent>().each([=](auto object, LuaScriptComponent& lsc) {
			lsc.lua = std::make_shared<sol::state>();


			lsc.lua->open_libraries(sol::lib::base);

			sol::usertype<Object> object_type = lsc.lua->new_usertype<Object>("object",
				// 3 constructors
				sol::constructors<Object()>());
			
			sol::state_view obj = lsc.lua->set("this",  Object(object, this));


			sol::usertype<TransformComponent> transform_type = lsc.lua->new_usertype<TransformComponent>("transform", sol::constructors<TransformComponent()>());
			sol::usertype<SpriteRendererComponent> spriterender_type = lsc.lua->new_usertype<SpriteRendererComponent>("sprite", sol::constructors<SpriteRendererComponent()>());
			sol::usertype<CameraComponent> camera_type = lsc.lua->new_usertype<CameraComponent>("camera", sol::constructors<CameraComponent()>());
			sol::usertype<AnimationComponent> animation_type = lsc.lua->new_usertype<AnimationComponent>("animation", sol::constructors<AnimationComponent()>());
			sol::usertype<LuaScriptComponent> script_type = lsc.lua->new_usertype<LuaScriptComponent>("script", sol::constructors<LuaScriptComponent()>());

			
		

			//Log
			lsc.lua->set_function("Log", [](int arg) { SE_INFO(arg); });
			lsc.lua->set_function("Log", [](std::string arg) { SE_INFO(arg); });

			//Input
			lsc.lua->set_function("IsKeyPressed", [](int keycode) {return Input::IsKeyPressed(keycode); });
			lsc.lua->set_function("IsMouseButtonPressed", [](int button) {return Input::IsMouseButtonPressed(button); });
			lsc.lua->set_function("GetMouseX", []() {return Input::GetMouseX(); });
			lsc.lua->set_function("GetMouseY", []() {return Input::GetMouseY(); });

			//Util
			lsc.lua->set_function("SetResolution", [](int x, int y) {Renderer2D::SetRenderSize(x, y); });


			//EnTT
			{
				object_type.set_function("GetObject", [](Object& o, std::string uuid) {
					return o.GetScene()->GetObjectByUUID(std::stoull(uuid));
				});
				object_type.set_function("GetTransform", [](Object& o) {
					return &o.GetComponent<TransformComponent>();
				});
				object_type.set_function("GetSprite", [](Object& o) {
					return &o.GetComponent<SpriteRendererComponent>();
				});
				object_type.set_function("GetCamera", [](Object& o) {
					return &o.GetComponent<CameraComponent>();
				});
				object_type.set_function("GetAnimation", [](Object& o) {
					return &o.GetComponent<AnimationComponent>();
				});

				object_type.set_function("HasComponent", [](Object& o, std::string compname) {
					if (compname._Equal("Transform")) { return o.HasComponent<TransformComponent>(); }
					if (compname._Equal("Animation")) { return o.HasComponent<AnimationComponent>(); }
					if (compname._Equal("SpriteRenderer")) { return o.HasComponent<SpriteRendererComponent>(); }
					if (compname._Equal("Camera")) { return o.HasComponent<CameraComponent>(); }
					if (compname._Equal("Script")) { return o.HasComponent<LuaScriptComponent>(); }
				});
			}
			//Transform
			{
				transform_type.set_function("SetPosition", [&](TransformComponent& o, float x, float y) {
					o.Translation.x = x;
					o.Translation.y = y;
				});
				
				transform_type.set_function("SetPositionX", [&](TransformComponent& o, float x) {
					o.Translation.x = x;
				});

				transform_type.set_function("SetPositionY", [&](TransformComponent& o, float y) {
					o.Translation.y = y;
				});

				transform_type.set_function("SetRotation", [&](TransformComponent& o, float y) {
					o.Rotation.y = y;
				});

				transform_type.set_function("GetRotation", [](TransformComponent& o) {
					return	o.Rotation.y;
				});

				transform_type.set_function("GetPositionX", [](TransformComponent& o) {
					return	o.Translation.x;
				});

				transform_type.set_function("GetPositionY", [](TransformComponent& o) {
					return	o.Translation.y;
				});

				transform_type.set_function("Rotate", [](TransformComponent& o, float amount) {
					o.Rotation.z += amount;
				});

				transform_type.set_function("TranslateX", [](TransformComponent& o, float amount) {
					o.Translation.x += amount;
				});

				transform_type.set_function("TranslateY", [](TransformComponent& o, float amount) {
					o.Translation.y += amount;
				});
			}

			//SpriteRenderer
			{
				spriterender_type.set_function("GetColor", [](SpriteRendererComponent& o) {
					return std::make_tuple(o.Color.r, o.Color.g, o.Color.b, o.Color.a);
				});

				spriterender_type.set_function("SetColorRGB", [&](SpriteRendererComponent& o, float r, float g, float b) {
					o.Color = glm::vec4(r, g, b, 1.f);
				});

				spriterender_type.set_function("SetColorRGBA", [&](SpriteRendererComponent& o, float r, float g, float b, float a) {
					o.Color = glm::vec4(r, g, b, a);
				});

				spriterender_type.set_function("SetSprite", [&](SpriteRendererComponent& o, std::string path) {
					o.Texture = Texture2D::Create(path);
					o.Texture_Path = path;
				});

				spriterender_type.set_function("IsFlippedX", [&](SpriteRendererComponent& o) {
					return o.flipX;
				});

				spriterender_type.set_function("SetFlippedX", [&](SpriteRendererComponent& o, bool flip) {
					o.flipX = flip;
				});

				spriterender_type.set_function("GetLayer", [&](SpriteRendererComponent& o, int layer) {
					return o.Layer;
				});

				spriterender_type.set_function("SetLayer", [&](SpriteRendererComponent& o, int layer) {
					o.Layer = layer;
					});
			}

			//Camera
			{
				camera_type.set_function("GetSize", [&](CameraComponent& o) {
					return o.Camera.GetOrthographicSize();
				});

				camera_type.set_function("GetNearClip", [&](CameraComponent& o) {
					return o.Camera.GetOrthographicNearClip();
				});

				camera_type.set_function("GetFarClip", [&](CameraComponent& o) {
					return o.Camera.GetOrthographicFarClip();
				});

				camera_type.set_function("SetSize", [&](CameraComponent& o, float size) {
					return o.Camera.SetOrthographicSize(size);
				});

				camera_type.set_function("SetNearClip", [&](CameraComponent& o, float dist) {
					return o.Camera.SetOrthographicNearClip(dist);
				});

				camera_type.set_function("SetFarClip", [&](CameraComponent& o, float dist) {
					return o.Camera.SetOrthographicFarClip(dist);
				});
			}

			//Animation
			{
				animation_type.set_function("GetCurrentFrame", [&](AnimationComponent& o) {
					return o.currframe;
				});

				animation_type.set_function("SetCurrentFrame", [&](AnimationComponent& o, int frame) {
					o.currframe = frame;
					});

				animation_type.set_function("GetTotalFrames", [&](AnimationComponent& o) {
					return o.frames;
					});

				animation_type.set_function("SetTotalFrames", [&](AnimationComponent& o, int totalFrames) {
					o.frames = totalFrames;
					});

				animation_type.set_function("GetCurrentFrame", [&](AnimationComponent& o) {
					return o.currframe;
				});

				animation_type.set_function("GetFps", [&](AnimationComponent& o) {
					return o.fps;
				});

				animation_type.set_function("SetFps", [&](AnimationComponent& o, int fps) {
					o.fps = fps;
				});

				animation_type.set_function("IsLooping", [&](AnimationComponent& o) {
					return o.loop;
				});

				animation_type.set_function("SetLooping", [&](AnimationComponent& o, bool loop) {
					o.loop = loop;
				});

				animation_type.set_function("IsPlayOnAwake", [&](AnimationComponent& o) {
					return o.playOnAwake;
				});

				animation_type.set_function("SetPlayOnAwake", [&](AnimationComponent& o, bool playOnAwake) {
					o.playOnAwake = playOnAwake;
				});

				animation_type.set_function("Play", [&](AnimationComponent& o) {
					o.play = true;
				});

				animation_type.set_function("Stop", [&](AnimationComponent& o) {
					o.play = false;
				});

				animation_type.set_function("StopAndReset", [&](AnimationComponent& o) {
					o.play = false;
					o.currframe = 0;
				});

				lsc.lua->set_function("KeyCode", [](std::string arg) {
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

			auto result = lsc.lua->safe_script_file(
				lsc.script_path, sol::script_pass_on_error);
			if (!result.valid()) {
				sol::error err = result;
				SE_CORE_ERROR(err.what());
			}

			sol::state& s = *lsc.lua.get();
			lsc.luaFunc_OnStart = std::make_shared<sol::protected_function>(s["OnStart"]);
			lsc.luaFunc_OnUpdate = std::make_shared<sol::protected_function>(s["OnUpdate"]);
			lsc.luaFunc_OnEnd = std::make_shared<sol::protected_function>(s["OnEnd"]);


			sol::protected_function_result r = lsc.luaFunc_OnStart->call();
			if (!r.valid()) {
				sol::error err = r;
				std::string what = err.what();
				SE_CORE_ERROR("OnStart Error" + what);
			}

			for (Script_Var& v : lsc.variables) {
				switch (v.type) {
					case VARTYPE::STRING: {
						lsc.lua->set(v.name.c_str(), v.value); break;
					}
					case VARTYPE::BOOL: {
						lsc.lua->set(v.name.c_str(), v.value._Equal("true")); break;
					}
					case VARTYPE::FLOAT: {
						lsc.lua->set(v.name.c_str(), (float)std::stod(v.value)); break;
					}
					case VARTYPE::INT: {
						lsc.lua->set(v.name.c_str(), std::stoi(v.value)); break;
					}
				}
			}
		});
	}

	Object Scene::GetObjectByUUID(UUID uuid) {
		Object o;
		auto groupTag = m_Registry.group<TagComponent>(entt::get<TransformComponent>);
		m_Registry.view<TagComponent>().each([&](auto object, TagComponent& tc) {
				if (uuid ==  tc.uuid) {
					o = Object(object, this);
				}
			});
		return o;
	}
}