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

		void Save() override;
		void Load() override;

		std::string UniqueId() { return "ME_MR"; }
		static std::string StaticId() { return "ME_MR";}

	public:
		Ref<Mesh> m_Mesh;
		Ref<TransformComponent> m_TransformComp;
	};
}