#pragma once
#include "ScreenComponent.h"
#include <string>
#include "glm/glm.hpp"
#include "imgui/imgui.h"

	void Draw2DTransformUI(glm::vec2* position, std::string name) {
		ImGui::PushID(name.c_str());
		ImGui::Text(name.c_str());
		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });
		ImVec2 buttonSize(19.0f, 19.0f);
		int x;
		int y;
		x = position->x;
		y = position->y;
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.7f,0.0f,0.0f,1.0f });
		if (ImGui::Button("X", buttonSize)) {
			x = 0;
		}
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(40.f);
		ImGui::DragInt("##X", &x, 1.0f);
		ImGui::PopStyleVar();


		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f,0.7f,0.0f,1.0f });
		if (ImGui::Button("Y", buttonSize)) {
			y = 0;

		}
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(40.f);
		ImGui::DragInt("##Y", &y, 1.0f);
		ImGui::PopStyleVar();

		position->x = x;
		position->y = y;
		ImGui::PopID();
	}
	
	ScreenComponent::ScreenComponent(MechEngine::Object* object) :
		MechEngine::Component(object),
		m_Resolution({ 1920,1080 })
	{
		if (m_ParentObject->HasComponent<MechEngine::MeshRendererComponent>() == -1) {
			m_ParentObject->AddComponent<MechEngine::MeshRendererComponent>();
		}

	}

	void ScreenComponent::OnUpdate() {
	
	}

	void ScreenComponent::DrawUI() {
		ImGui::Text("Screen Component");
		Draw2DTransformUI(&m_Resolution, "Resolution");
		ImGui::Separator();
	}

	void ScreenComponent::Save() {
		if (!MechEngine::Serialization::ReadyForWrite()) {
			ME_ERROR("ERROR - ScreenMesh: Serializer not ready for Write");
			return;
		}

	}	

	void ScreenComponent::Load() {
		
	}

	void ScreenComponent::OnDelete() {
		
	}