#include "sepch.h"
#include "Object.h"
#include "Components.h"

namespace SurfEngine {
	Object::Object(entt::entity handle, Scene* scene) 
		: m_ObjectHandle(handle), m_Scene(scene)
	{
	}

	UUID Object::GetUUID() const { return GetComponent<TagComponent>().uuid; }
	const std::string& Object::GetName() const { return GetComponent<TagComponent>().Tag; }
}