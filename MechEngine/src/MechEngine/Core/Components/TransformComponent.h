#pragma once
#include "MechEngine/Core/Object.h"
#include "MechEngine/Core/Components/Component.h"
#include "MechEngine/Core/Transform.h"


namespace MechEngine {

	class TransformComponent : public Component {
	public:
		TransformComponent(Object* object);
		void Draw3DTransformUI(glm::vec3* position, std::string name, int defaultValue);
		void DrawUI() override;
		void OnUpdate() override;
	public:
		Transform transform;
	};
}