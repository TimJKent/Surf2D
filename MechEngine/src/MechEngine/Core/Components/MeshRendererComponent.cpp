#pragma once
#include "mepch.h"
#include "MeshRendererComponent.h"
#include "MechEngine/Core/Object.h"
#include "MechEngine/Renderer/Renderer2D.h"
#include "MechEngine/Core/Components/ComponentUtility.h"
#include "imgui.h"

namespace MechEngine {
	MeshRendererComponent::MeshRendererComponent(Object* object) : Component(object) {
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

		if (m_ParentObject->HasComponent<TransformComponent>() == -1) {
			m_ParentObject->AddComponent<TransformComponent>();
		}

		int slot = m_ParentObject->HasComponent<TransformComponent>();
		m_TransformComp = m_ParentObject->GetComponent<TransformComponent>(slot).get();
		m_TransformComp->RequiredCounter++;

	}

	MeshRendererComponent::~MeshRendererComponent() {
		
	}
	

	void MeshRendererComponent::DrawUI() {
		ImGui::Text("Mesh Renderer");	
		m_TransformComp = RequireType<TransformComponent>("Transform", this, m_TransformComp);
		
		ImGui::Separator();
	}

	void MeshRendererComponent::OnUpdate()  {
		if (m_TransformComp) {
			MechEngine::Transform t = m_TransformComp->transform;
			t.SetPosition(t.GetPosition() / 1000.f);
			//t2.SetPosition(t2.GetPosition() / 1000.f);
			//t.SetScale({ 1.02f, 1.02f, 1.0f });
			//t.Translate(0.f, 0.f, -0.01f);
			//	MechEngine::Renderer2D::DrawMesh(t, m_Mesh, 3);
			Renderer2D::DrawMesh(t, m_Mesh, 2);
		}
	}

	void MeshRendererComponent::Save() {
		if (!Serialization::ReadyForWrite()) {
			ME_ERROR("ERROR - ScreenMesh: Serializer not ready for Write");
			return;
		}
	}

	void MeshRendererComponent::Load() {
	
	}

	void MeshRendererComponent::OnDelete() {
		--m_TransformComp->RequiredCounter;
	}
}