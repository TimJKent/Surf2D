#pragma once
#include "MechEngine/Core/Object.h"
#include "MechEngine/Core/Components/Component.h"
#include "MechEngine/Renderer/Mesh.h"
#include "MechEngine/Renderer/Material.h"
#include "MechEngine/Core/Transform.h"
#include "MechEngine/Renderer/Renderer2D.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace MechEngine {


	class MeshRendererComponenet : public Component {
	public:
		MeshRendererComponenet() {
			m_Mesh.reset(new MechEngine::Mesh());
			m_Mesh->AddVertex({ -1.920f / 2,	-1.080f / 2,	0.f }, { 0.f, 0.f }, MechEngine::Color(0.f, 0.f, 0.f, 1.f));
			m_Mesh->AddVertex({ 1.920f / 2,	-1.080f / 2,	0.f }, { 1.f, 0.f }, MechEngine::Color(0.f, 0.f, 0.f, 1.f));
			m_Mesh->AddVertex({ 1.920f / 2,	 1.080f / 2, 0.f }, { 1.f, 1.f }, MechEngine::Color(0.f, 0.f, 0.f, 1.f));
			m_Mesh->AddVertex({ -1.920f / 2,	 1.080f / 2, 0.f }, { 0.f, 1.f }, MechEngine::Color(0.f, 0.f, 0.f, 1.f));
			m_Mesh->AddIndex(0);
			m_Mesh->AddIndex(1);
			m_Mesh->AddIndex(2);
			m_Mesh->AddIndex(2);
			m_Mesh->AddIndex(3);
			m_Mesh->AddIndex(0);
		//	GetParentObject()->SetName("Poopy:0");
		}
		void DrawUI() override {
			ImGui::Text("Mesh Renderer");
			ImGui::Separator();

		}
		void OnUpdate() override {
			MechEngine::Transform t;
			//t.SetPosition(t.GetPosition() / 1000.f);
			//t2.SetPosition(t2.GetPosition() / 1000.f);
			//t.SetScale({ 1.02f, 1.02f, 1.0f });
			//t.Translate(0.f, 0.f, -0.01f);
			//	MechEngine::Renderer2D::DrawMesh(t, m_Mesh, 3);
			Renderer2D::DrawMesh(t, m_Mesh, 2);
		}

	public:
		Ref<Mesh> m_Mesh;
	};
}