#pragma once
#include <string>
#include "MechEngine/Core/Core.h"
#include "Component.h"

namespace MechEngine {
	template <typename T>
	Ref <T> RequireType(std::string label, Component* baseComponent, Ref<T> component) {
		int counter = 0;
		static std::string currSelected = label + std::to_string(0);
		if (ImGui::BeginCombo(label.c_str(), currSelected.c_str())) {
			for (int i = 0; i < baseComponent->m_ParentObject->GetNumberOfComponents(); i++) {
				if (baseComponent->m_ParentObject->ComponentIsTypeOf<TransformComponent>(i)) {
					std::string selectableName = label + std::to_string(counter++);
					if (ImGui::Selectable(selectableName.c_str())) {
						currSelected = selectableName;
						ImGui::EndCombo();
						return baseComponent->m_ParentObject->GetComponent<T>(i);
					}
				}
			}
			ImGui::EndCombo();
		}
		return component;
	}
}
