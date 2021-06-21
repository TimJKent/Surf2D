#pragma once
#include "MechEngine/Core/Object.h"
#include "MechEngine/Core/Components/Component.h"
#include "MechEngine/Core/Components/TransformComponent.h"
#include "MechEngine/Renderer/Mesh.h"


namespace MechEngine {

	class MeshRendererComponenet : public Component {
	public:
		MeshRendererComponenet(Object* object);

		void DrawUI() override;

		void OnUpdate() override;

	public:
		Ref<Mesh> m_Mesh;
		Ref<TransformComponent> m_TransformComp;
	};
}