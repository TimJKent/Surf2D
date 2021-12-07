#include "sepch.h"
#include "Scene.h"

#include "Components.h"
#include "SurfEngine/Renderer/Renderer2D.h"
#include "SurfEngine/Scenes/Object.h"

#include <glm/glm.hpp>

namespace SurfEngine {
	Scene::Scene(){
	}

	Scene::~Scene() {
		
	}

	void Scene::OnUpdateRuntime(Timestep ts) {

		//Update Scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto object, auto& nsc){
				if (!nsc.Instance) {
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Object = { object, this };
					nsc.Instance->OnCreate();
				}

				nsc.Instance->OnUpdate(ts);
			});
		}

		
		m_Registry.view<CameraComponent>().each([=](auto object, CameraComponent& cc) {
				if (!m_sceneCamera) {
					m_sceneCamera.reset(&cc.Camera);
				}
				
		});

		auto groupCamera = m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
		for (auto entity : groupCamera) {
			auto [camera, transform] = groupCamera.get<CameraComponent, TransformComponent>(entity);
			camera.Camera.SetView(transform.GetTransform());
		}
		if (m_sceneCamera) {
			Renderer2D::BeginScene(m_sceneCamera);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group) {
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				if (sprite.Texture) {
					Renderer2D::DrawQuad(transform.GetTransform(), std::make_shared<SpriteRendererComponent>(sprite));
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

	void Scene::DeleteObject(Object o)
	{
		m_Registry.destroy(o);
	}

	void Scene::OnUpdateEditor(Timestep ts, Ref<Camera> camera, bool draw_grid) {
		Renderer2D::BeginScene(camera);
		if (draw_grid)
			Renderer2D::DrawBackgroundGrid(1);

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group) {
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			if (sprite.Texture) {
				Renderer2D::DrawQuad(transform.GetTransform(), std::make_shared<SpriteRendererComponent>(sprite));
			}
			else {
				Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
			}
		}

		Renderer2D::EndScene();
	}
}