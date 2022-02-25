#include "Panel_Hierarchy.h"

#include "SurfEngine/Scenes/Components.h"
#include "SurfEngine/Scenes/Object.h"
#include "SurfEngine/Scenes/Scene.h"
#include "ProjectManager.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace SurfEngine {

	Panel_Hierarchy::Panel_Hierarchy() {
		add_icon = Texture2D::Create("res\\textures\\icon_add.png");

	}

	void Panel_Hierarchy::OnImGuiRender(){

		if (ImGui::Begin("Hierarchy",NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration)) {
			if (ProjectManager::IsActiveScene()) {
				char* buff = new char[40];
				float buttonWidth = 12.f;
				std::strcpy(buff, "search");
				ImGui::InputText("##searc",buff, 40);
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (buttonWidth + 18.f));
				if (ImGui::ImageButton((ImTextureID)(uint64_t)add_icon->GetRendererID(), { buttonWidth ,buttonWidth })) {
					auto& scene = ProjectManager::GetActiveScene();
					scene->CreateObject("New Game Object");
				}
				ImGui::Separator();
				ProjectManager::GetActiveScene()->m_Registry.each([&](auto objectId) {
					DrawObjectNode(objectId);
				});
				delete[] buff;
			}
		}
		ImGui::End();

	}

	void Panel_Hierarchy::DrawObjectNode(entt::entity enttid) {
		auto& tag = ProjectManager::GetActiveScene()->m_Registry.get<TagComponent>(enttid).Tag;
		bool selectHighlight = false;
		if (ProjectManager::GetSelectedObject().get()) {
			selectHighlight = *ProjectManager::GetSelectedObject().get() == enttid;
		}
		ImGuiTreeNodeFlags flags = ((selectHighlight) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)enttid, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			ProjectManager::SetSelectedObject(std::make_shared<Object>(Object(enttid, ProjectManager::GetActiveScene().get())));
		}

		bool entityDeleted = false;
		bool duplicateEntity = false;

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
			if (ImGui::MenuItem("Delete Entity")){
				entityDeleted = true;
			}
			if (ImGui::MenuItem("Duplicate Entity")) {
			
				duplicateEntity = true;
			}

			ImGui::EndPopup();
		}

		if (entityDeleted)
		{
			ProjectManager::GetActiveScene()->DeleteObject(enttid);
			ProjectManager::ClearSelectedObject();
		}
		if (duplicateEntity)
		{
			ProjectManager::GetActiveScene()->DuplicateObject(enttid);
		}
	}
}
