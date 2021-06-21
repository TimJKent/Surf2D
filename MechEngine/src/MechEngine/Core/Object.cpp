#pragma once

#include "mepch.h"
#include "MechEngine/Core/Object.h"
#include "MechEngine/Core/Components/Component.h"
#include "MechEngine/Core/Components/TransformComponent.h"
#include "MechEngine/Core/Components/TransformComponent.cpp"
#include "MechEngine/Core/Components/MeshRendererComponent.h"
#include "MechEngine/Core/Components/MeshRendererComponent.cpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace MechEngine {

	Object::Object():
		m_Name("New Object"),
		m_Enabled(true),
		m_Components()
	{}
	
	void Object::DrawUI() {
		char* name = new char[m_Name.size() + 10];
		strcpy(name, m_Name.c_str());
		ImGui::PushID("Visible");
		ImGui::Checkbox("", &m_Enabled);
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::PushID("Name");
		ImGui::InputText("", name, 20);
		ImGui::PopID();
		SetName(name);
		ImGui::Separator();

		for (int i = 0; i < m_Components.size(); i++) {
			bool alwaysFalse = false;
			ImGui::PushID("enabled" + i);
			if (m_Enabled) {
				ImGui::Checkbox("", &m_Components[i]->IsEnabled);
			}
			else {
				ImGui::Checkbox("", &alwaysFalse);
				alwaysFalse = false;
			}
			ImGui::PopID();
			ImGui::SameLine();
			m_Components[i]->DrawUI();
		}

		if (ImGui::Button("AddComponent")) { ImGui::OpenPopup("ComponentMenu"); }
		if (ImGui::BeginPopup("ComponentMenu"))
		{
			if (ImGui::Selectable("Transform")) {
				AddComponent<TransformComponent>();
			}
			if (ImGui::Selectable("Mesh Renderer")) {
				AddComponent<MeshRendererComponenet>();
			}
			ImGui::EndPopup();
		}
	}

	void Object::OnUpdate() {
		if (m_Enabled) {
			for (int i = 0; i < m_Components.size(); i++) {
				if (m_Components[i]->IsEnabled)
					m_Components[i]->OnUpdate();
			}
		}
	}

	const const std::string& Object::GetName() const {
		return m_Name;
	}

	void Object::SetName(const std::string& name) {
		m_Name = name;
	}
}