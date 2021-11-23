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

	void Scene::OnUpdate(Timestep ts) {

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


		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group) {
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
		}
	}

	unsigned int Scene::ObjectCount() {
		return m_Registry.size();
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
}