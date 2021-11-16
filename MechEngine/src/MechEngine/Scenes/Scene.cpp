#include "mepch.h"
#include "Scene.h"

#include "Components.h"
#include "MechEngine/Renderer/Renderer2D.h"
#include "MechEngine/Scenes/Object.h"

#include <glm/glm.hpp>

namespace MechEngine {
	Scene::Scene(){
	}

	Scene::~Scene() {
		
	}

	void Scene::OnUpdate(Timestep ts) {
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group) {
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

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

	void Scene::DeleteObject(Object o)
	{
		m_Registry.destroy(o);
	}
}