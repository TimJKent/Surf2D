#include "Panel_Hierarchy.h"

#include "SurfEngine/Scenes/Components.h"
#include "SurfEngine/Scenes/Object.h"
#include "SurfEngine/Scenes/Scene.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace SurfEngine {

	Panel_Hierarchy::Panel_Hierarchy(const Ref<Scene>& scene) {
		m_ActiveScene = scene;
		m_SelectedObjectContext = {};
	}

	void Panel_Hierarchy::SetActiveScene(Ref<Scene>&scene){
		m_ActiveScene = scene;
		m_SelectedObjectContext = {};
	}

	void Panel_Hierarchy::OnImGuiRender(){
		if (ImGui::Begin("Hierarchy")) {
			m_ActiveScene->m_Registry.each([&](auto objectId) {
				Object object{ objectId, m_ActiveScene.get() };
				DrawObjectNode(object);
			});
		}
		ImGui::End();
	}

	Object Panel_Hierarchy::GetSelectedObject(){
		return m_SelectedObjectContext;
	}

	void Panel_Hierarchy::SetSelectedObject(Object object){
		m_SelectedObjectContext = object;
	}

	void Panel_Hierarchy::DrawObjectNode(Object object) {
		
		auto& tag = object.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectedObjectContext == object) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)object, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectedObjectContext = object;
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
			m_ActiveScene->DeleteObject(object);
			if (m_SelectedObjectContext == object)
				m_SelectedObjectContext = {};
		}
		
		

		
	}
}
