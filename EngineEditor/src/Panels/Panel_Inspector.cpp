#include "mepch.h"

#include "Panel_Inspector.h"
#include "MechEngine/Scenes/Components.h"
#include "MechEngine/Renderer/Renderer2D.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace MechEngine {
	void Panel_Inspector::OnImGuiRender() {
		if (ImGui::Begin("Inspector")) {
			if (m_panel_hierarchy->m_SelectedObjectContext) {
				Object o = m_panel_hierarchy->m_SelectedObjectContext;
				if (o.HasComponent<TagComponent>()) { DrawComponentTag(o); }
				if (o.HasComponent<TransformComponent>()) { DrawComponentTransform(o); }
				if (o.HasComponent<SpriteRendererComponent>()) { DrawComponentSpriteRenderer(o); }
			}
		}
		ImGui::End();
	}

	void Panel_Inspector::DrawComponentTag(Object o) {
		TagComponent& tc = o.GetComponent<TagComponent>();
		
		//Draw Name
		{
			ImGui::Text("Name");
			ImGui::SameLine();
			char* nameBuffer = new char[tc.Tag.size() + 16];
			std::strcpy(nameBuffer, tc.Tag.c_str());
			size_t nameBufferSize = tc.Tag.size() + 16;
			ImGui::PushID("NameTextField");
			if (ImGui::InputText("", nameBuffer, nameBufferSize, ImGuiInputTextFlags_EnterReturnsTrue)) {
				if (std::strcmp(nameBuffer, "") != 0) { tc.Tag = nameBuffer; }
			}
			ImGui::PopID();
			delete[] nameBuffer;
		}

		//Draw UUID
		if (m_DebugMode) {
			ImGui::Text("UUID:");
			ImGui::SameLine();
			std::string uuidstring = std::to_string(tc.uuid);
			char* uuidbuff = new char[uuidstring.size()];
			std::strcpy(uuidbuff, uuidstring.c_str());
			ImGui::PushID("UUIDTextField");
			ImGui::InputText("", uuidbuff, uuidstring.size(),ImGuiInputTextFlags_ReadOnly);
			ImGui::PopID();
			delete[] uuidbuff;
		}

		ImGui::Separator();
		
	}


	void Panel_Inspector::DrawComponentTransform(Object o) {
		TransformComponent& tc = o.GetComponent<TransformComponent>();
		ImGui::Text("Transform");
		ImGui::NewLine();
		float pos[2] = { tc.Translation.x,-tc.Translation.y };
		ImGui::Text("Position");
		
		ImGui::DragFloat2("",pos,0.25f);
		tc.Translation.x = pos[0];
		tc.Translation.y = -pos[1];
		ImGui::Separator();
	}

	void Panel_Inspector::DrawComponentSpriteRenderer(Object o) {
		SpriteRendererComponent& tc = o.GetComponent<SpriteRendererComponent>();
		ImGui::Text("Sprite Renderer");
		ImGui::NewLine();
		float color[4] = { tc.Color.r,tc.Color.g, tc.Color.b, tc.Color.a };
		ImGui::Text("Base Color");
		ImGui::ColorEdit4("", color);
		tc.Color = { color[0],color[1], color[2], color[3] };
		ImGui::Separator();
	}
}