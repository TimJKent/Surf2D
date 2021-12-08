#include "sepch.h"

#include "Panel_Inspector.h"
#include "SurfEngine/Scenes/Components.h"
#include "SurfEngine/Renderer/Renderer2D.h"
#include "SurfEngine/Core/PlatformUtils.h"
#include "ProjectManager.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace SurfEngine {
	void Panel_Inspector::OnImGuiRender() {
		if (ImGui::Begin("Inspector")) {
			if (ProjectManager::IsSelectedObject()) {
				Object o = ProjectManager::GetSelectedObject();
					if (o.HasComponent<TagComponent>()) { DrawComponentTag(o); }
					if (o.HasComponent<TransformComponent>()) { DrawComponentTransform(o); }
					if (o.HasComponent<SpriteRendererComponent>()) { DrawComponentSpriteRenderer(o); }
					if (o.HasComponent<CameraComponent>()) { DrawComponentCamera(o); }
					if (o.HasComponent<NativeScriptComponent>()) { DrawComponentScript(o); }
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
		ImGui::NewLine();
		int layer_value = tc.Layer;
		ImGui::InputInt("Layer",&layer_value,1,5);
		tc.Layer = layer_value < 0 ? tc.Layer : layer_value;
		ImGui::NewLine();
		ImGui::Text("Sprite");
		if(tc.Texture)

		ImGui::Image((ImTextureID)(uint64_t)tc.Texture->GetRendererID(), ImVec2{128,128},ImVec2(1,1), ImVec2(0, 0),ImVec4(1,1,1,1), ImVec4(0,0,0,1));
		ImGui::SameLine();
		if (ImGui::Button("Select Sprite")) {
			std::string img_path = FileDialogs::OpenFile(ProjectManager::GetPath(), "Image (*.png)\0*.png\0");
			if (!img_path.empty()) {
				tc.Texture_Path = img_path;
				tc.Texture = Texture2D::Create(img_path);
				SE_CORE_WARN("Changed Sprite to: " + img_path);
			}
		}

		ImGui::Separator();
	}

	void Panel_Inspector::DrawComponentCamera(Object o) {
		CameraComponent& cc = o.GetComponent<CameraComponent>();
		auto& camera = cc.Camera;
		float csize = cc.Camera.GetOrthographicSize();
		float cnear = cc.Camera.GetOrthographicNearClip();
		float cfar  = cc.Camera.GetOrthographicFarClip();


		ImGui::Text("Camera");
		ImGui::NewLine();
		ImGui::Text("Size"); ImGui::SameLine();
		ImGui::InputFloat("##Size",&csize);
		ImGui::Text("Near Clip"); ImGui::SameLine();
		ImGui::InputFloat("##Near", &cnear);
		ImGui::Text("Far Clip"); ImGui::SameLine();
		ImGui::InputFloat("##Far", &cfar);

		camera.SetOrthographicSize(csize);
		camera.SetOrthographicNearClip(cnear);
		camera.SetOrthographicFarClip(cfar);

		ImGui::Separator();
	}

	void Panel_Inspector::DrawComponentScript(Object o) {
		ImGui::Text("Custom Script");
		ImGui::Separator();
	}
}