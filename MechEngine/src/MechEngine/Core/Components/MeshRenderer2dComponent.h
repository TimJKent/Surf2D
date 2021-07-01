#pragma once
#include "MechEngine/Core/Object.h"
#include "MechEngine/Core/Components/Component.h"
#include "MechEngine/Core/Components/TransformComponent.h"
#include "MechEngine/Renderer/Mesh.h"


namespace MechEngine {

	class MeshRenderer2dComponent : public Component {
	public:
		MeshRenderer2dComponent(Object* object);
		~MeshRenderer2dComponent();
		void DrawUI() override;
		void OnUpdate() override;
		void OnDelete() override;

		void Save() override;
		void Load() override;

		void SaveRefComponents() override;
		void LoadRefComponents() override;


		std::string UniqueId() { return "ME_MR2d"; }
		static std::string StaticId() { return "ME_MR2d"; }

	public:
		Ref<Mesh> m_Mesh;
		TransformComponent* m_TransformComp;
	};
}