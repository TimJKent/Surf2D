#include "Panel_Hierarchy.h"

#include "SurfEngine/Scenes/Components.h"
#include "SurfEngine/Scenes/Object.h"
#include "SurfEngine/Scenes/Scene.h"
#include "SurfEngine/Core/Input.h"
#include "SurfEngine/Core/KeyCodes.h"
#include "../Util/ProjectManager.h"
#include "../Util/SceneManager.h"
#include "SurfEngine/Scenes/ObjectSerializer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace SurfEngine {
	Panel_Hierarchy::Panel_Hierarchy() {
		add_icon = Texture2D::Create("res\\textures\\icon_add.png");

	}

	void Panel_Hierarchy::OnImGuiRender(){
		if (ImGui::Begin("Hierarchy",NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration)) {
			if (SceneManager::IsActiveScene()) {
				if (Input::IsKeyPressed(SE_KEY_ESCAPE)) {
					SceneManager::ClearSelectedObject();
				}

				char* buff = new char[40];
				float buttonWidth = 12.f;
				std::strcpy(buff, "search");
				ImGui::InputText("##search",buff, 40);
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (buttonWidth + 18.f));
				if (ImGui::ImageButton((ImTextureID)(uint64_t)add_icon->GetRendererID(), { buttonWidth ,buttonWidth })) {
					auto& scene = SceneManager::GetActiveScene();
					scene->CreateObject("New Game Object");
				}
				ImGui::Separator();
				auto group = SceneManager::GetActiveScene()->m_Registry.group<TagComponent>(entt::get<TransformComponent>);
				group.sort<TagComponent>([](const TagComponent& lhs, const TagComponent& rhs) {
					return lhs.Tag.compare(rhs.Tag) < 0;
				});
				for (auto entity : group) {
					auto [tag, transform] = group.get<TagComponent, TransformComponent>(entity);
					if (SceneManager::GetActiveScene()->m_Registry.valid(entity)) {
						if(transform.parent == nullptr)
							DrawObjectNode(Object(entity, SceneManager::GetActiveScene().get()));
					}
				}
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
						TransformComponent* ref = &SceneManager::GetActiveScene()->GetObjectByUUID(uuid).GetComponent<TransformComponent>();
						
						ref->SetParent(nullptr);
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::PopID();
			}
		}
		ImGui::End();
	}

	void Panel_Hierarchy::DrawObjectNode(Object object) {
		TagComponent& tag = SceneManager::GetActiveScene()->m_Registry.get<TagComponent>(object);
		TransformComponent& tc = SceneManager::GetActiveScene()->m_Registry.get<TransformComponent>(object);

		ImGui::PushID(tag.Tag.c_str());

		bool entityDeleted = false;
		bool duplicateEntity = false;
		
		bool selectHighlight = false;

		selectHighlight = SceneManager::IsObjectSelected(std::make_shared<Object>(object));
		
		ImGuiTreeNodeFlags flags = ((selectHighlight) ? ImGuiTreeNodeFlags_Selected : 0) | ((tc.children.size() > 0) ? ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Leaf) | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

		
		if (ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)object, flags, tag.Tag.c_str()))
		{
			if (ImGui::IsItemClicked())
			{
				if (Input::IsKeyPressed(SE_KEY_LEFT_CONTROL)) {
					Ref<Object> obj_to_select = std::make_shared<Object>(Object(object, SceneManager::GetActiveScene().get()));
					if (SceneManager::IsObjectSelected(obj_to_select)) {
						SceneManager::RemoveObjectFromSelection(obj_to_select);
					}else{
						SceneManager::PushObjectToSelection(obj_to_select);
					}
				}
				else {
					SceneManager::ClearSelectedObject();
					SceneManager::PushObjectToSelection(std::make_shared<Object>(Object(object, SceneManager::GetActiveScene().get())));
				}
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
					TransformComponent* ref = &SceneManager::GetActiveScene()->GetObjectByUUID(uuid).GetComponent<TransformComponent>();
					ref->SetParent(&tc);
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginPopupContextItem(tag.uuid.ToString().c_str()))
			{
				if (ImGui::MenuItem("Create Asset")) {
					ObjectSerializer serializer = ObjectSerializer();
					serializer.Serialize(
						ProjectManager::GetPath()+"\\" + tag.Tag + ".asset",
						object
					);
				}
				if (ImGui::MenuItem("Duplicate")) {
					for (int i = 0; i < SceneManager::GetSelectedObjectCount(); i++) {
						SceneManager::GetActiveScene()->DuplicateObject(*SceneManager::GetSelectedObject()[i].get());
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Delete")) {
					for (int i = 0; i < SceneManager::GetSelectedObjectCount(); i++) {
						SceneManager::GetActiveScene()->DeleteObject(*SceneManager::GetSelectedObject()[i].get());
					}
					SceneManager::ClearSelectedObject();
				}
				
				ImGui::EndPopup();
			}
			for (int i = 0; i < tc.children.size(); i++) {
				DrawObjectNode(tc.children[i]->gameObject);
			}

			ImGui::TreePop();
		}
		ImGui::PopID();
	}
}
