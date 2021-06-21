#pragma once
#include "mepch.h"
#include "MechEngine/Core/Components/Component.h"
#include "MechEngine/Core/Object.h"

namespace MechEngine {
	Component::Component(Object* object) : m_ParentObject(object){
	}
}