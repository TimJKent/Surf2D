#include "sepch.h"
#include "Scene.h"

#include "Components.h"
#include "SurfEngine/Core/KeyCodes.h"
#include "SurfEngine/Renderer/Renderer2D.h"
#include "SurfEngine/Scenes/Object.h"

#include <glm/glm.hpp>

#include "mono-2.0/mono/metadata/assembly.h"
#include "mono-2.0/mono/jit/jit.h"

namespace SurfEngine {

	void DoSomething()
	{
		SE_CORE_INFO("Hello World");
	}

	Scene::Scene(){
		m_Registry = entt::registry();
		MonoDomain* domain;
		domain = mono_jit_init("startup.exe");

		MonoAssembly* assembly = mono_domain_assembly_open(domain, "startup.exe");

		if (!assembly) {
			SE_CORE_ERROR("Assembly not loaded!");
		}
		else {
			char** path = new char* ("startup.exe");
			auto retval = mono_jit_exec(domain, assembly, 1 - 1, path + 1);
		}
		
		

		mono_jit_cleanup(domain);
	}

	Scene::~Scene() {
		
	}

	void Scene::OnUpdateRuntime(Timestep ts) {
		//Create Scene Cameras from Camera Components
		m_Registry.view<CameraComponent>().each([=](auto object, CameraComponent& cc) {
			m_sceneCamera = std::make_shared<SceneCamera>(cc.Camera);
			});

		if (m_sceneCamera) {
			Renderer2D::BeginScene(m_sceneCamera.get());
			
			//Do Animations
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
			glm::vec4 color = { 0.6f,0.6f,0.6f,1.0f };
			float size = camera.Camera.GetOrthographicSize();
			float height = (size / 2)*(Renderer2D::GetRenderTargetSize().x/Renderer2D::GetRenderTargetSize().y);
			Renderer2D::DrawBox({ -height,-size / 2 }, { -height,size / 2 }, { height,size / 2 }, { height,-size / 2 }, transform.GetTransform(), color);
			Renderer2D::DrawGizmo(transform.GetTransform(), Renderer2D::GetGizmo(), Renderer2D::GetGizmoColorInActive());
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
						Renderer2D::DrawGizmo(tc.GetTransform(), Renderer2D::GetGizmo(), Renderer2D::GetGizmoColorActive());
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

	Object Scene::DuplicateObject(entt::entity o)
	{
		Object old = Object(o,this);
		auto& oldtc = old.GetComponent<TransformComponent>();
		Object duplicate = CreateObject(old.GetComponent<TagComponent>().Tag + "_dup");
		duplicate.GetComponent<TransformComponent>().Scale = oldtc.Scale;
		duplicate.GetComponent<TransformComponent>().Rotation = oldtc.Rotation;
		duplicate.GetComponent<TransformComponent>().Translation = oldtc.Translation;
		if (old.HasComponent<CameraComponent>()) { duplicate.AddComponent<CameraComponent>(old.GetComponent<CameraComponent>()); }
		if (old.HasComponent<AnimationComponent>()) { duplicate.AddComponent<AnimationComponent>(old.GetComponent<AnimationComponent>()); }
		if (old.HasComponent<SpriteRendererComponent>()) { duplicate.AddComponent<SpriteRendererComponent>(old.GetComponent<SpriteRendererComponent>()); }

		return duplicate;
	}

	void Scene::DeleteObject(entt::entity o)
	{
		Object obj = Object(o, this);
		TransformComponent& tc = obj.GetComponent<TransformComponent>();
		
		for (TransformComponent* c : tc.children) {
			DeleteObject(c->gameObject);
		}

		if (tc.parent)
			tc.parent->RemoveChild(&tc);
		m_Registry.destroy(o);

	}



	void Scene::OnSceneEnd() {
		m_IsPlaying = false;
		m_sceneCamera = nullptr;
		SE_CORE_INFO("Scene \"" + m_name + ".scene\" Ended");
	}

	void Scene::OnSceneStart() {
		SE_CORE_INFO("Scene \"" + m_name+ ".scene\" Started");
		m_IsPlaying = true;

		m_Registry.view<AnimationComponent>().each([=](auto object, AnimationComponent& ac) {
				ac.play = ac.playOnAwake;
			});


		m_Registry.view<CameraComponent>().each([=](auto object, CameraComponent& cc) {
			m_sceneCamera = std::make_shared<SceneCamera>(cc.Camera);
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