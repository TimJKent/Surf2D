#include "sepch.h"
#include "Scene.h"

#include "Components.h"
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
					if (sprite.reflective) {
						Renderer2D::DrawReflectiveQuad(transform.GetTransform(), std::make_shared<SpriteRendererComponent>(sprite), Renderer2D::GetOutputAsTextureId());
					}
					else {
						Renderer2D::DrawQuad(transform.GetTransform(), std::make_shared<SpriteRendererComponent>(sprite), sprite.currFrame, sprite.totalFrames);
					}
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


		m_Registry.view<LuaScriptComponent>().each([=](auto object, auto& nsc) {
			nsc.OnUpdate(ts, &Object(object, this));
			});

		auto groupCamera = m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
		for (auto entity : groupCamera) {
			auto [camera, transform] = groupCamera.get<CameraComponent, TransformComponent>(entity);
			camera.Camera.m_Transform = transform.GetTransform();
			camera.Camera.UpdateView();
		}
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

	void Scene::DeleteObject(entt::entity o)
	{
		m_Registry.destroy(o);
	}

	void Scene::OnUpdateEditor(Timestep ts, Ref<SceneCamera> camera, bool draw_grid) {
		SetSceneCamera(camera);
		Renderer2D::BeginScene(camera.get());
		if (draw_grid)
			Renderer2D::DrawBackgroundGrid(1);



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
				float frame = 1.0f;
				float totalFrames = 8.0f;


				Renderer2D::DrawQuad(transform.GetTransform(), std::make_shared<SpriteRendererComponent>(sprite), sprite.currFrame, sprite.totalFrames);
			}
			else {
				Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
			}
		}

		Renderer2D::EndScene();
	}

	void Scene::OnSceneEnd() {
		m_IsPlaying = false;

			m_Registry.view<LuaScriptComponent>().each([=](auto object, auto& nsc) {
				nsc.OnDestroy();
			});
		
	}

	void Scene::OnSceneStart() {
		m_IsPlaying = true;
			m_Registry.view<AnimationComponent>().each([=](auto object, AnimationComponent& ac) {
					ac.play = ac.playOnAwake;
				});
			m_Registry.view<LuaScriptComponent>().each([=](auto object, LuaScriptComponent& lsc) {
				lsc.OnCreate(&Object(object,this));
				lsc.UpdateVariables();
			});
		
	}
}