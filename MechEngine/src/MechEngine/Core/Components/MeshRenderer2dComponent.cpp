#pragma once
#include "mepch.h"
#include "MeshRenderer2dComponent.h"
#include "MechEngine/Core/Object.h"
#include "MechEngine/Renderer/Renderer2D.h"
#include "MechEngine/Core/Components/ComponentUtility.h"
#include "imgui.h"

namespace MechEngine {
	MeshRenderer2dComponent::MeshRenderer2dComponent(Object* object) : Component(object) {
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

	MeshRenderer2dComponent::~MeshRenderer2dComponent() {

	}


	void MeshRenderer2dComponent::DrawUI() {
		
		ImGui::Text("Mesh Renderer 2d");
		ImGui::PushID("MeshRenderer2d");
		m_TransformComp = RequireType<TransformComponent>("Transform", this, m_TransformComp);
		ImGui::PopID();

		ImGui::Separator();
	}

	void MeshRenderer2dComponent::OnUpdate() {
		if (m_TransformComp && !Renderer2D::GetRenderMode()) {
			MechEngine::Transform t = m_TransformComp->transform;
			t.SetPosition(t.GetPosition() / 1000.f);
			//t2.SetPosition(t2.GetPosition() / 1000.f);
			//t.SetScale({ 1.02f, 1.02f, 1.0f });
			//t.Translate(0.f, 0.f, -0.01f);
			//	MechEngine::Renderer2D::DrawMesh(t, m_Mesh, 3);
			Renderer2D::DrawMesh(t, m_Mesh, 2);
		}
	}

	void MeshRenderer2dComponent::Save() {
		if (!Serialization::ReadyForWrite()) {
			ME_ERROR("ERROR - ScreenMesh: Serializer not ready for Write");
			return;
		}
	}

	void MeshRenderer2dComponent::Load() {

	}

	void MeshRenderer2dComponent::SaveRefComponents() {
		Serialization::SERIAL_WRITE((std::string)m_TransformComp->UniqueId().c_str());
		Serialization::SERIAL_WRITE((int)m_ParentObject->GetComponentSlot(m_TransformComp->IdNumber));
	}

	void MeshRenderer2dComponent::LoadRefComponents() {
		std::string temp = "";
		int slot = 0;
		Serialization::SERIAL_READ((std::string*) & temp);
		Serialization::SERIAL_READ((int*)&slot);
		m_TransformComp = m_ParentObject->GetComponent<TransformComponent>(slot).get();
	}

	void MeshRenderer2dComponent::OnDelete() {
		--m_TransformComp->RequiredCounter;
	}
}