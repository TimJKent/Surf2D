#include "sepch.h"
#include "Object.h"

namespace SurfEngine {
	Object::Object(entt::entity handle, Scene* scene) 
		: m_ObjectHandle(handle), m_Scene(scene)
	{
	}
}