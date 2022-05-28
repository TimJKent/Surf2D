#include "Panel_Hierarchy.h"

#include "SurfEngine/Scenes/Components.h"
#include "SurfEngine/Scenes/Object.h"
#include "SurfEngine/Scenes/Scene.h"
#include "../Util/ProjectManager.h"

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
				ImGui::InputText("##search",buff, 40);
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (buttonWidth + 18.f));
				if (ImGui::ImageButton((ImTextureID)(uint64_t)add_icon->GetRendererID(), { buttonWidth ,buttonWidth })) {
					auto& scene = ProjectManager::GetActiveScene();
					scene->CreateObject("New Game Object");
				}
				ImGui::Separator();
				ProjectManager::GetActiveScene()->m_Registry.each([&](auto objectId) {
					TransformComponent& tc = ProjectManager::GetActiveScene()->m_Registry.get<TransformComponent>(objectId);
					if(!tc.parent)
						DrawObjectNode(objectId);
				});
				delete[] buff;
				ImGui::PushID("rspace");
				ImGui::InvisibleButton("##dd",ImGui::GetContentRegionAvail());

				if (ImGui::BeginDragDropTarget())
				{

					char* data;
					const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("objectitem");
					if (payload) {
						data = new char[payload->DataSize + 1];
						memcpy((char*)&data[0], payload->Data, payload->DataSize);

						UUID uuid = std::stoull(data);
						TransformComponent* ref = &ProjectManager::GetActiveScene()->GetObjectByUUID(uuid).GetComponent<TransformComponent>();
						if (ref->parent) {
							for (int i = 0; i < ref->parent->children.size(); i++) {
								if (ref->parent->children[i] == ref) {
									ref->parent->children.erase(ref->parent->children.begin() + i);
									i = ref->parent->children.size();
								}
							}
							ref->parent = nullptr;
						}
					}

					ImGui::EndDragDropTarget();
				}
				ImGui::PopID();
			
			}
		}
		ImGui::End();

	}

	void Panel_Hierarchy::DrawObjectNode(entt::entity enttid) {
		TagComponent& tag = ProjectManager::GetActiveScene()->m_Registry.get<TagComponent>(enttid);
		TransformComponent& tc = ProjectManager::GetActiveScene()->m_Registry.get<TransformComponent>(enttid);

		ImGui::PushID(tag.Tag.c_str());

		bool selectHighlight = false;

		if (ProjectManager::GetSelectedObject().get()) {
			selectHighlight = *ProjectManager::GetSelectedObject().get() == enttid;
		}

		
		bool opened = false;
		
		if (tc.children.size()>0) {
			ImGuiTreeNodeFlags flags = ((selectHighlight) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)enttid, flags, tag.Tag.c_str());
		}
		else {
			ImGuiTreeNodeFlags flags = (ImGuiTreeNodeFlags_Leaf | ((selectHighlight) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_SpanAvailWidth);
			ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)enttid, flags, tag.Tag.c_str());
			ImGui::TreePop();
		}



		if (ImGui::IsItemClicked())
		{
			ProjectManager::SetSelectedObject(std::make_shared<Object>(Object(enttid, ProjectManager::GetActiveScene().get())));
		}

		if (ImGui::BeginDragDropSource()) {
			ImGui::Text(tag.Tag.c_str());
			std::string data = tag.uuid.ToString();
			data += '\0';
			ImGui::SetDragDropPayload("objectitem", data.c_str(), data.length());
			ImGui::EndDragDropSource();
		}
		
		if (ImGui::BeginDragDropTarget())
		{
			
			char* data;
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("objectitem");
			if (payload) {
				data = new char[payload->DataSize + 1];
				memcpy((char*)&data[0], payload->Data, payload->DataSize);
		
				UUID uuid = std::stoull(data);
				TransformComponent* ref = &ProjectManager::GetActiveScene()->GetObjectByUUID(uuid).GetComponent<TransformComponent>();
				

				for (int i = 0; i < ref->children.size(); i++) {
					if (ref->children[i] == &tc) {
						if (ref->parent) {
							ref->children[i]->parent = ref->parent;
						}
						else {
							ref->children[i]->parent = nullptr;
						}
						ref->children.erase(ref->children.begin() + i);
					}
				}
				
				if (ref->parent) {
					for (int i = 0; i < ref->parent->children.size(); i++) {
						if (ref->parent->children[i] == ref) {
							ref->parent->children.erase(ref->parent->children.begin()+i);
							i = ref->parent->children.size();
						}
					}
				}
				
				tc.parent = ref->parent;
				ref->parent = &tc;
				tc.children.push_back(ref);
				
			}
		
			ImGui::EndDragDropTarget();
		}



		bool entityDeleted = false;
		bool duplicateEntity = false;

		if (opened)
		{
			for (int i = 0; i < tc.children.size(); i++) {
				DrawObjectNode(tc.children[i]->gameObject);
			}
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
		ImGui::PopID();
	}
}
