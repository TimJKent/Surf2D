#include "mepch.h"
#include "Object.h"

namespace MechEngine {
	Object::Object(entt::entity handle, Scene* scene) 
		: m_ObjectHandle(handle), m_Scene(scene), m_uuid()
	{}
}