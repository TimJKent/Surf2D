#pragma once
#include <string>
#include "MechEngine/Core/Core.h"
#include "Component.h"

namespace MechEngine {

	static int id = 0;

	static int GetNewIdNumber() {
		return id++;
	}

	template <typename T>
	T* RequireType(std::string label, Component* baseComponent, Component* component) {
		int counter = 0;
		std::string currSelected = label + std::to_string(component->IdNumber);
		if (ImGui::BeginCombo(label.c_str(), currSelected.c_str())) {
			for (int i = 0; i < baseComponent->m_ParentObject->GetNumberOfComponents(); i++) {
				if (baseComponent->m_ParentObject->ComponentIsTypeOf<T>(i)) {
					Ref<Component> c = baseComponent->m_ParentObject->GetComponent<T>(i);
					std::string selectableName = label + std::to_string(c->IdNumber);

					if (ImGui::Selectable(selectableName.c_str())) {
						currSelected = selectableName;
						ImGui::EndCombo();
						component->RequiredCounter--;
						component = baseComponent->m_ParentObject->GetComponent<T>(i).get();
						component->RequiredCounter++;
						return baseComponent->m_ParentObject->GetComponent<T>(i).get();
					}
				
				}
			}
			ImGui::EndCombo();
		}
		return (T*)component;
	}
}
