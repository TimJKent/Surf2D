#include "Panel_Hierarchy.h"

#include "SurfEngine/Scenes/Components.h"
#include "SurfEngine/Scenes/Object.h"
#include "SurfEngine/Scenes/Scene.h"
#include "ProjectManager.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace SurfEngine {

	Panel_Hierarchy::Panel_Hierarchy() {
	}

	void Panel_Hierarchy::OnImGuiRender(){

		if (ImGui::Begin("Hierarchy")) {
			if (ProjectManager::IsActiveScene()) {
				ProjectManager::GetActiveScene()->m_Registry.each([&](auto objectId) {
					Object object{ objectId, ProjectManager::GetActiveScene().get() };
					DrawObjectNode(object);
				});
			}
		}
		ImGui::End();

	}

	void Panel_Hierarchy::DrawObjectNode(Object object) {
		
		auto& tag = object.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((ProjectManager::GetSelectedObject() == object) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)object, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			ProjectManager::SetSelectedObject(object);
		}

		bool entityDeleted = false;

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (entityDeleted)
		{
			ProjectManager::GetActiveScene()->DeleteObject(object);
			ProjectManager::ClearSelectedObject();
		}
	}
}
