#include "sepch.h"

#include "Panel_Inspector.h"
#include "SurfEngine/Scenes/Components.h"
#include "SurfEngine/Renderer/Renderer2D.h"
#include "SurfEngine/Core/PlatformUtils.h"
#include "../Util/ProjectManager.h"

#include <filesystem>
#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace SurfEngine {
	void Panel_Inspector::OnImGuiRender() {
		if (ImGui::Begin("Inspector")) {
			if (ProjectManager::IsSelectedObject()) {
				Ref<Object> o = ProjectManager::GetSelectedObject();
					if (o->HasComponent<TagComponent>()) { DrawComponentTag(o); }
					if (o->HasComponent<TransformComponent>()) { DrawComponentTransform(o); }
					if (o->HasComponent<SpriteRendererComponent>()) { DrawComponentSpriteRenderer(o); }
					if (o->HasComponent<AnimationComponent>()) { DrawComponentAnimation(o); }
					if (o->HasComponent<CameraComponent>()) { DrawComponentCamera(o); }
					if (o->HasComponent<ScriptComponent>()) { DrawComponentScript(o); }
					ImGui::NewLine();
					if (ImGui::Button("Add Component")) {
						ImGui::OpenPopupContextItem("Add Component",ImGuiMouseButton_Left);
					}
			}
			else if (!ProjectManager::s_SelectedPath.empty()) {
				std::filesystem::path path = ProjectManager::s_SelectedPath;
				char* nameBuffer = new char[path.stem().string().length() + 16];
				std::strcpy(nameBuffer, path.stem().string().c_str());
				size_t nameBufferSize = path.stem().string().size() + 16;
				ImGui::PushID("NameTextField");
				if (ImGui::InputText("", nameBuffer, nameBufferSize, ImGuiInputTextFlags_EnterReturnsTrue)) {
					if (std::strcmp(nameBuffer, "") != 0) { 
						std::error_code error;
						std::filesystem::path old_path = path;
						std::filesystem::path new_path = path.replace_filename(nameBuffer).string() + path.extension().string();
						std::filesystem::rename(old_path, new_path, error);
						if (error) {
							SE_CORE_ERROR("Failed to Rename File Error Code:" + error.message());
						}
						else {
							ProjectManager::SetSelectedPath(new_path.string());
						}
					}
				}
				ImGui::PopID();
				delete[] nameBuffer;

				if (path.extension().string().compare(".cs") == 0) {
					std::string line;
					std::ifstream myfile(path.string());
					if (myfile.is_open())
					{
						while (getline(myfile, line))
						{
							ImGui::Text(line.c_str());
						}
						myfile.close();
					}
					else {
						SE_CORE_WARN("Unable to open file in Inspector");
					}
					myfile.close();
				}
			}
		}
		if (ImGui::BeginPopupContextItem("Add Component"))
		{
			Ref<Object> o = ProjectManager::GetSelectedObject();
				if (ImGui::MenuItem("Sprite Renderer")) {
					if (!o->HasComponent<SpriteRendererComponent>()) { o->AddComponent<SpriteRendererComponent>(); }
				}
				if (ImGui::MenuItem("Animation")) {
					if (!o->HasComponent<AnimationComponent>()) { o->AddComponent<AnimationComponent>(); }
				}
				if (ImGui::MenuItem("Camera")) {
					if (!o->HasComponent<CameraComponent>()) { o->AddComponent<CameraComponent>(); }
				}
				if (ImGui::MenuItem("Script")) {
					if (!o->HasComponent<ScriptComponent>()) { o->AddComponent<ScriptComponent>(); }
				}
				ImGui::EndPopup();
		}
		ImGui::End();
	}

	void Panel_Inspector::DrawComponentTag(Ref<Object> o) {
		TagComponent& tc = o->GetComponent<TagComponent>();
		
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

	void Panel_Inspector::DrawComponentTransform(Ref<Object> o) {
		TransformComponent& tc = o->GetComponent<TransformComponent>();
		ImGui::Text("Transform");
		ImGui::NewLine();
		float pos[2] = { tc.Translation.x,-tc.Translation.y };
		float rot[1] = {  tc.Rotation.z };
		float scale[3] = { tc.Scale.x, tc.Scale.y, tc.Scale.z };

		ImGui::Text("Position");
		ImGui::DragFloat2("##pos", pos, 0.25f);

		ImGui::Text("Rotation");
		ImGui::DragFloat("##rot", rot, 0.25f);

		ImGui::Text("Scale");

		ImGui::DragFloat3("##scale",scale,0.25f);
		tc.Translation.x = pos[0];
		tc.Translation.y = -pos[1];
		tc.Scale.x = scale[0];
		tc.Scale.y = scale[1];
		tc.Scale.z = scale[2];
		tc.Rotation.z = rot[0];
		ImGui::Separator();
	}

	void Panel_Inspector::DrawComponentSpriteRenderer(Ref<Object> o) {
		ImGui::PushID("SpritePopup");
		ImGui::Text("Sprite Renderer");
		ImGui::OpenPopupOnItemClick("RemoveComp");
	
		

		SpriteRendererComponent& sr = o->GetComponent<SpriteRendererComponent>();
		ImGui::NewLine();
		float color[4] = { sr.Color.r,sr.Color.g, sr.Color.b, sr.Color.a };
		ImGui::Text("Base Color");
		ImGui::ColorEdit4("", color);
		sr.Color = { color[0],color[1], color[2], color[3] };
		ImGui::NewLine();
		int layer_value = sr.Layer;
		ImGui::InputInt("Layer",&layer_value,1,5);
		sr.Layer = layer_value < 0 ? sr.Layer : layer_value;
		ImGui::NewLine();
		ImGui::Text("Sprite");

		ImGui::PushID("SpriteSelector");
		if (sr.Texture) {
			ImGui::Image((ImTextureID)(uint64_t)sr.Texture->GetRendererID(), ImVec2{128,128},ImVec2(0,1), ImVec2(1, 0),ImVec4(1,1,1,1), ImVec4(0,0,0,1));
			if (ImGui::BeginDragDropTarget())
			{
				char* path;
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("fileitem");
				if (payload) {
					path = new char[payload->DataSize + 1];
					memcpy((char*)&path[0], payload->Data, payload->DataSize);
					sr.Texture_Path = path;
					sr.Texture = Texture2D::Create(sr.Texture_Path);
					SE_CORE_WARN("Changed Sprite to: " + sr.Texture_Path);
				}
				ImGui::EndDragDropTarget();
			}
		}
		ImGui::PopID();
		
		ImGui::SameLine();
		if (ImGui::Button("Select Sprite")) {
			std::string img_path = FileDialogs::OpenFile(ProjectManager::GetPath(), "Image (*.png)\0*.png\0");
			if (!img_path.empty()) {
				sr.Texture_Path = img_path;
				sr.Texture = Texture2D::Create(img_path);
				SE_CORE_WARN("Changed Sprite to: " + img_path);
			}
		}
		ImGui::Checkbox("Flip X", &sr.flipX);
		ImGui::Checkbox("Reflective", &sr.reflective);
		if (!sr.Texture) { sr.reflective = false; }
		ImGui::Separator();

		if (ImGui::BeginPopup("RemoveComp")) {
			if (ImGui::Selectable("Remove")) {
				o->RemoveComponent<SpriteRendererComponent>();
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();

	}

	void Panel_Inspector::DrawComponentCamera(Ref<Object> o) {

		ImGui::PushID("Camera");
		ImGui::Text("Camera");
		ImGui::OpenPopupOnItemClick("RemoveComp");

		CameraComponent& cc = o->GetComponent<CameraComponent>();
		auto& camera = cc.Camera;
		float csize = cc.Camera.GetOrthographicSize();
		float cnear = cc.Camera.GetOrthographicNearClip();
		float cfar  = cc.Camera.GetOrthographicFarClip();
		
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

		if (ImGui::BeginPopup("RemoveComp")) {
			if (ImGui::Selectable("Remove")) {
				o->RemoveComponent<CameraComponent>();
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	void Panel_Inspector::DrawComponentAnimation(Ref<Object> o) {

		ImGui::PushID("Animation");
		ImGui::Text("Animation");
		ImGui::OpenPopupOnItemClick("RemoveComp");

		AnimationComponent& ac = o->GetComponent<AnimationComponent>();

		ImGui::NewLine();
		ImGui::Text("Frames"); ImGui::SameLine();
		ImGui::InputInt("##Frames", &ac.frames);

		ImGui::Text("CurrFrame"); ImGui::SameLine();
		ImGui::InputInt("##CurrFrames", &ac.currframe);

		ImGui::Text("Fps"); ImGui::SameLine();
		ImGui::InputInt("##Fps", &ac.fps);

		ImGui::Checkbox("PlayOnAwake", &ac.playOnAwake);

		ImGui::Checkbox("Loop", &ac.loop);

		

		ImGui::Separator();

		if (ImGui::BeginPopup("RemoveComp")) {
			if (ImGui::Selectable("Remove")) {
				o->RemoveComponent<AnimationComponent>();
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	void Panel_Inspector::DrawComponentScript(Ref<Object> o) {

		ImGui::PushID("Script");
		ImGui::Text("Script");
		ImGui::OpenPopupOnItemClick("RemoveComp");

		ScriptComponent& sc = o->GetComponent<ScriptComponent>();

		ImGui::NewLine();
		
		//Draw Name
		{
			ImGui::Text("Script");
			ImGui::SameLine();
			char* nameBuffer = new char[sc.path.size() + 16];
			std::strcpy(nameBuffer, sc.path.c_str());
			size_t nameBufferSize = sc.path.size() + 16;
			ImGui::PushID("NameTextField");
			if (ImGui::InputText("", nameBuffer, nameBufferSize, ImGuiInputTextFlags_EnterReturnsTrue)) {
				if (std::strcmp(nameBuffer, "") != 0) { sc.path = nameBuffer; }
			}
			ImGui::PopID();
			delete[] nameBuffer;
		}

		ImGui::Separator();

		if (ImGui::BeginPopup("RemoveComp")) {
			if (ImGui::Selectable("Remove")) {
				o->RemoveComponent<AnimationComponent>();
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}
}