#pragma once

#include "mepch.h"
#include "Object.h"
#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include "Components/MeshRendererComponent.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace MechEngine {

	Object::Object():
		transform(),
		m_Name("New Object"),
		m_Hidden(false),
		m_Components()
	{}
	
	void Object::DrawUI() {
		bool visible = !m_Hidden;
		char* name = new char[m_Name.size() + 10];
		strcpy(name, m_Name.c_str());
		ImGui::PushID("Visible");
		ImGui::Checkbox("", &visible);
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::PushID("Name");
		ImGui::InputText("", name, 20);
		ImGui::PopID();
		SetName(name);
		SetHidden(!visible);
		ImGui::Separator();

		for (int i = 0; i < m_Components.size(); i++) {
			m_Components[i]->DrawUI();
		}

		if (ImGui::Button("AddComponent")) { ImGui::OpenPopup("ComponentMenu"); }
		if (ImGui::BeginPopup("ComponentMenu"))
		{
			if (ImGui::Selectable("Transform")) {
				m_Components.push_back(new TransformComponenet());
			}
			if (ImGui::Selectable("Mesh Renderer")) {
				m_Components.push_back(new TransformComponenet());
			}
			ImGui::EndPopup();
		}
	}

	void Object::OnUpdate() {
		for (int i = 0; i < m_Components.size(); i++) {
			m_Components[i]->OnUpdate();
		}
	}

	const const std::string& Object::GetName() const {
		return m_Name;
	}

	void Object::SetName(const std::string& name) {
		m_Name = name;
	}

	const const bool Object::GetHidden() const {
		return m_Hidden;
	}

	void Object::SetHidden(bool hide) {
		m_Hidden = hide;
	}
}