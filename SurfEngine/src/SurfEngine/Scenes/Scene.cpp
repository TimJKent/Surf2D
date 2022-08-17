#include "sepch.h"
#include "Scene.h"

#include "Components.h"
#include "SurfEngine/Core/KeyCodes.h"
#include "SurfEngine/Renderer/Renderer2D.h"
#include "SurfEngine/Scenes/Object.h"

#include <filesystem>

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"

#include "SurfEngine/Scripting/ScriptEngine.h"
#include "SurfEngine/Physics/PhysicsEngine.h"

#include <glm/glm.hpp>

namespace SurfEngine {

	Scene::Scene(){
		m_Registry = entt::registry();
	}

	Scene::~Scene() {
	
	}

	Object Scene::CreateObject(const std::string& name)
	{
		std::string objName = (name.empty()) ? "NewGameObject" : name;
		entt::entity entity = m_Registry.create();
		Object object = { entity, this };
		object.AddComponent<TransformComponent>(object);
		object.AddComponent<TagComponent>(objName);
		return object;
	}

	Object Scene::CreateObject(const std::string& name, UUID uuid)
	{
		std::string objName = (name.empty()) ? "NewGameObject" : name;
		entt::entity entity = m_Registry.create();
		Object object = { entity, this };
		object.AddComponent<TransformComponent>(object);
		object.AddComponent<TagComponent>(objName, uuid);
		return object;
	}

	Object Scene::GetObjectByUUID(UUID uuid) {
		Object o;
		m_Registry.view<TagComponent>().each([&](auto object, TagComponent& tc) {
			if (uuid == tc.uuid) {
				o = Object(object, this);
			}
			});
		return o;
	}

	Object Scene::GetObjectByName(std::string name) {
		Object o;
		m_Registry.view<TagComponent>().each([&](auto object, TagComponent& tc) {
			if (name._Equal(tc.Tag)) {
				o = Object(object, this);
			}
			});
		return o;
	}

	Object Scene::DuplicateObject(Object source_obj)
	{
		Object duplicate = CreateObject(source_obj.GetComponent<TagComponent>().Tag + "_dup", SurfEngine::UUID());
		TransformComponent& oldtc = source_obj.GetComponent<TransformComponent>();
		duplicate.GetComponent<TransformComponent>().Scale = oldtc.Scale;
		duplicate.GetComponent<TransformComponent>().Rotation = oldtc.Rotation;
		duplicate.GetComponent<TransformComponent>().Translation = oldtc.Translation;
		if (source_obj.HasComponent<CameraComponent>()) { duplicate.AddComponent<CameraComponent>(source_obj.GetComponent<CameraComponent>());}
		if (source_obj.HasComponent<AnimationComponent>()) { duplicate.AddComponent<AnimationComponent>(source_obj.GetComponent<AnimationComponent>()); }
		if (source_obj.HasComponent<SpriteRendererComponent>()) { duplicate.AddComponent<SpriteRendererComponent>(source_obj.GetComponent<SpriteRendererComponent>()); }
		if (source_obj.HasComponent<ScriptComponent>()) { duplicate.AddComponent<ScriptComponent>(source_obj.GetComponent<ScriptComponent>()); }
		if (source_obj.HasComponent<BoxColliderComponent>()) { duplicate.AddComponent<BoxColliderComponent>(source_obj.GetComponent<BoxColliderComponent>()); }
		if (source_obj.HasComponent<CircleColliderComponent>()) { duplicate.AddComponent<CircleColliderComponent>(source_obj.GetComponent<CircleColliderComponent>()); }
		if (source_obj.HasComponent<RigidbodyComponent>()) { duplicate.AddComponent<RigidbodyComponent>(source_obj.GetComponent<RigidbodyComponent>()); }

		return duplicate;
	}

	void Scene::DeleteObject(Object obj)
	{
		TransformComponent& tc = obj.GetComponent<TransformComponent>();

		for (TransformComponent* c : tc.children) {
			DeleteObject(c->gameObject);
		}

		if (tc.parent)
			tc.parent->RemoveChild(&tc);
		m_Registry.destroy(obj);
	}

	std::size_t Scene::ObjectCount() {
		return m_Registry.size();
	}

	void Scene::OnSceneStart() {
		SE_CORE_INFO("Scene \"" + m_name + ".scene\" Started");

		m_Registry.view<AnimationComponent>().each([=](auto object, AnimationComponent& ac) {
			ac.play = ac.playOnAwake;
			});

		//TODO: Added Main Camera Code
		m_Registry.view<CameraComponent>().each([=](auto object, CameraComponent& cc) {
			m_sceneCamera = std::make_shared<SceneCamera>(cc.Camera);
			});

		m_Registry.view<BoxColliderComponent>().each([=](auto object, BoxColliderComponent& cc) {
			if (!cc.physics_material_path.empty()) {
				cc.physics_material = SceneSerializer::GetPhysicsMaterialFromPath(cc.physics_material_path);
			}
		});

		m_Registry.view<CircleColliderComponent>().each([=](auto object, CircleColliderComponent& cc) {
			if (!cc.physics_material_path.empty()) {
				cc.physics_material = SceneSerializer::GetPhysicsMaterialFromPath(cc.physics_material_path);
			}
		});

		ScriptEngine::SceneStart();
		PhysicsEngine::OnPhysics2DStart(this);

		m_Registry.view<ScriptComponent>().each([&](auto object, ScriptComponent& cc) {
			ScriptEngine::SetCurrentScene(this);
			std::filesystem::path path = cc.path;
			cc.monoclass = ScriptClass("", path.stem().string().c_str());
			if (cc.monoclass.monoclass) {
				MonoClass* monoclassG = mono_class_get_parent(cc.monoclass.monoclass);
				MonoMethod* method_Constructor = mono_class_get_method_from_name(monoclassG, "SetGameObject", 1);
				cc.script_class_instance = cc.monoclass.CreateInstance();

				void* args[1];
				args[0] = ScriptEngine::CreateMonoString(Object(object, this).GetComponent<TagComponent>().uuid.ToString().c_str());
				cc.monoclass.InvokeMethod(cc.script_class_instance, method_Constructor, args);

				MonoClassField* field;
				void* iter = NULL;
				while ((field = mono_class_get_fields(cc.monoclass.monoclass, &iter))) {
					std::string name = mono_field_get_name(field);
					for (int i = 0; i < cc.variables.size(); i++) {
						if (cc.variables[i].name._Equal(name)) {
							if (cc.variables[i].type._Equal("double")) {
								double d = std::stod(cc.variables[i].user_value);
								mono_field_set_value(cc.script_class_instance, field, &d);
							}
							if (cc.variables[i].type._Equal("int")) {
								int integer = std::stoi(cc.variables[i].user_value);
								mono_field_set_value(cc.script_class_instance, field, &integer);
							}
							if (cc.variables[i].type._Equal("string")) {
								mono_field_set_value(cc.script_class_instance, field, &cc.variables[i].user_value);
							}
							if (cc.variables[i].type._Equal("bool")) {
								bool b = cc.variables[i].user_value._Equal("true");
								mono_field_set_value(cc.script_class_instance, field, &b);
							}
							break;
						}
					}
				}
				
				cc.monoclass.InvokeMethod(cc.script_class_instance, cc.monoclass.GetMethod("OnStart", 0));
			}
			else {
				SE_CORE_WARN("SCRIPT_ENGINE: Failed to Load Script [{0}]",path.filename().string());
			}

		});

		m_IsPlaying = true;
	}

	void Scene::OnUpdateRuntime(Timestep ts) {	
	
		//TODO: Added Main Camera Code
		m_Registry.view<CameraComponent>().each([=](auto object, CameraComponent& cc) {
			m_sceneCamera = std::make_shared<SceneCamera>(cc.Camera);
			});

		PhysicsEngine::OnPhysics2DUpdate(ts,this);
		

		m_Registry.view<ScriptComponent>().each([=](auto object, ScriptComponent& cc) {
			ScriptEngine::SetCurrentScene(this);
			cc.monoclass.InvokeMethod(cc.script_class_instance, cc.monoclass.GetMethod("OnUpdate", 0));
		});

		if (m_sceneCamera) {
			Renderer2D::BeginScene(m_sceneCamera.get());
			

			auto animgroup = m_Registry.group<AnimationComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : animgroup) {
				auto [anim, sprite] = animgroup.get<AnimationComponent, SpriteRendererComponent>(entity);
				
				anim.Tick(ts);

				sprite.currFrame = anim.currframe;
				sprite.totalFrames = anim.frames;
			}

			//Draw Sprites
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
			SE_CORE_WARN("No Scene Camera Detected!");
		}

		//Update Camera
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

		auto groupCamera = m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
		for (auto entity : groupCamera) {
			auto [camera, transform] = groupCamera.get<CameraComponent, TransformComponent>(entity);
			float size = camera.Camera.GetOrthographicSize();
			float height = (size / 2)*(Renderer2D::GetRenderTargetSize().x/Renderer2D::GetRenderTargetSize().y);
			Renderer2D::DrawBox({ -height,-size / 2 }, { -height,size / 2 }, { height,size / 2 }, { height,-size / 2 }, transform.GetTransform(), { 0.6f,0.6f,0.6f,1.0f });
			Renderer2D::DrawGizmo(transform.GetTransform(), Renderer2D::GetGizmo(), Renderer2D::GetGizmoColorInActive());
		}

		m_Registry.view<TransformComponent>().each([=](auto object, TransformComponent& tc) {
			bool select = false;
			for (int i = 0; i < selected.size(); i++) {
				if (*selected[i].get() == object) {
					select = true;
				}
			}

			if (select) {
				if (!Object(object, this).HasComponent<CameraComponent>()) {
					float size = 0.5f;
					Renderer2D::DrawBox({ -size,-size }, { -size,size }, { size,size }, { size, -size }, tc.GetTransform(), { 1.0f,0.5f,0.0f,1.0f });
				}
				else {
					float size = Object(object, this).GetComponent<CameraComponent>().Camera.GetOrthographicSize();
					float height = (size / 2) * (Renderer2D::GetRenderTargetSize().x / Renderer2D::GetRenderTargetSize().y);
					Renderer2D::DrawBox({ -height,-size / 2 }, { -height,size / 2 }, { height,size / 2 }, { height,-size / 2 }, tc.GetTransform(), { 1.0f,0.5f,0.0f,1.0f });
					Renderer2D::DrawGizmo(tc.GetTransform(), Renderer2D::GetGizmo(), Renderer2D::GetGizmoColorActive());
				}
			}
			
		});

		auto view = m_Registry.view<BoxColliderComponent>();
		for(auto o : view)
		{
			glm::vec4 color = { 0.0f,1.0f,0.0f,0.33f };
			if (selected) {
				if (*selected.get() == o) {
					color.a = 1.0f;
				}
			}

			Object object = { o, this };
			BoxColliderComponent bc = object.GetComponent<BoxColliderComponent>();
			TransformComponent tc = object.GetComponent<TransformComponent>();
			Renderer2D::DrawBox({ -bc.Size.x / 2 + bc.Offset.x,-bc.Size.y / 2 - bc.Offset.y }, { bc.Size.x / 2 + bc.Offset.x,-bc.Size.y / 2 - bc.Offset.y }, { bc.Size.x / 2 + bc.Offset.x,bc.Size.y / 2 - bc.Offset.y }, { -bc.Size.x / 2 + bc.Offset.x, bc.Size.y / 2 - bc.Offset.y }, tc.GetTransform(), color);
		}

		auto cview = m_Registry.view<CircleColliderComponent>();
		for (auto o : cview)
		{
			glm::vec4 color = { 0.0f,1.0f,0.0f,0.33f };
			if (selected) {
				if (*selected.get() == o) {
					color.a = 1.0f;
				}
			}

			Object object = { o, this };
			CircleColliderComponent bc = object.GetComponent<CircleColliderComponent>();
			TransformComponent tc = object.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();
			transform = glm::translate(transform, { bc.Offset.x, -bc.Offset.y, 0.0f });
			transform = glm::scale(transform, { bc.Radius*2.f, bc.Radius*2.f, 1.0f });
			Renderer2D::DrawCircle(transform, color);
		}
		Renderer2D::EndScene();
	}

	void Scene::OnSceneEnd() {
		m_IsPlaying = false;
		m_sceneCamera = nullptr;

		ScriptEngine::SceneEnd();
		PhysicsEngine::OnPhysics2DStop();

		SE_CORE_INFO("Scene \"" + m_name + ".scene\" Ended");
	}
	
}