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
		m_Name("NewObject"),
		m_Enabled(true),
		m_Components()
	{}
	
	void Object::OnUpdate() {
		
		if (m_Enabled) {
			for (int i = 0; i < m_Components.size(); i++) {
				if (m_Components[i]->IsEnabled)
					m_Components[i]->OnUpdate();
			}
		}
	}

	void Object::RemoveComponent(int slot) {
		if (m_Components[slot]->RequiredCounter == 0) {
			m_Components[slot]->OnDelete();
			m_Components.erase(m_Components.begin() + slot);
		}
	}

	const const std::string& Object::GetName() const {
		return m_Name;
	}

	void Object::SetName(const std::string& name) {
		m_Name = name;
	}

	void Object::Save() {
		if (!Serialization::ReadyForWrite()) {
			return;
		}
		Serialization::SERIAL_WRITE(m_Name);
		Serialization::SERIAL_WRITE(m_Enabled);
		Serialization::SERIAL_WRITE((int)m_Components.size());
		for (int i = 0; i < m_Components.size(); i++) {
			Serialization::SERIAL_WRITE(m_Components[i]->UniqueId());
		}
		for (int i = 0; i < m_Components.size(); i++) {
			m_Components[i]->Save();
		}
	}

	void Object::Load(){
			if (!Serialization::ReadyForRead()) {
				ME_ERROR("ERROR - ScreenMesh: Serializer not ready for Read");
				return;
			}
			Serialization::SERIAL_READ(&m_Name);
			Serialization::SERIAL_READ(&m_Enabled);
	}

	void Object::LoadComponents(){
		for (int i = 0; i < m_Components.size(); i++) {
			m_Components[i]->Load();
		}
	}

	int Object::GetNumberOfComponents() {
		return m_Components.size();
	}
}