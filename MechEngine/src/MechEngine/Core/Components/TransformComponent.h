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
		void OnDelete() override;

		void Save() override;
		void Load() override;
		
		std::string UniqueId() { return "ME_T"; }
		static std::string StaticId() { return "ME_T"; }

	public:
		Transform transform;
	};
}