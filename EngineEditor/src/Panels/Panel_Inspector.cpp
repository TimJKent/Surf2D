#include "sepch.h"

#include "Panel_Inspector.h"
#include "SurfEngine/Scenes/Components.h"
#include "SurfEngine/Renderer/Renderer2D.h"
#include "SurfEngine/Core/PlatformUtils.h"
#include "../Util/ProjectManager.h"
#include "../Util/SceneManager.h"

#include <filesystem>
#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace SurfEngine {

	void DrawDragInputField(const std::string& label, std::string* path_ref, const std::string& extension) {
		std::filesystem::path nameBufferPath = *path_ref;
		size_t nameBufferSize = nameBufferPath.stem().string().size();
		char* nameBuffer = new char[nameBufferSize];
		std::strcpy(nameBuffer, nameBufferPath.stem().string().c_str());

		ImGui::NewLine();
		ImGui::Text(label.c_str());
		ImGui::SameLine();
		ImGui::PushID(label.c_str());


		ImGui::InputText("", nameBuffer, nameBufferSize, ImGuiInputTextFlags_ReadOnly);
		if (ImGui::BeginDragDropTarget())
		{
			char* cpath;
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("fileitem");
			if (payload) {
				cpath = new char[payload->DataSize + 1];
				memcpy((char*)&cpath[0], payload->Data, payload->DataSize);
				std::filesystem::path p = cpath;
				if (p.extension().string()._Equal(extension.c_str())) {
					*path_ref = cpath;
				}
			}
			ImGui::EndDragDropTarget();
		}


		ImGui::PopID();
		delete[] nameBuffer;
	}


	void Panel_Inspector::OnImGuiRender() {
		if (ImGui::Begin("Inspector")) {
			if (SceneManager::IsSelectedObject()) {
				Ref<Object> o = SceneManager::GetSelectedObject();
					if (o->HasComponent<TagComponent>()) { DrawComponentTag(o); }
					if (o->HasComponent<TransformComponent>()) { DrawComponentTransform(o); }
					if (o->HasComponent<SpriteRendererComponent>()) { DrawComponentSpriteRenderer(o); }
					if (o->HasComponent<AnimationComponent>()) { DrawComponentAnimation(o); }
					if (o->HasComponent<CameraComponent>()) { DrawComponentCamera(o); }
					if (o->HasComponent<RigidbodyComponent>()) { DrawComponentRigidBody(o); }
					if (o->HasComponent<BoxColliderComponent>()) { DrawComponentBoxCollider(o); }
					if (o->HasComponent<CircleColliderComponent>()) { DrawComponentCircleCollider(o); }
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
			Ref<Object> o = SceneManager::GetSelectedObject();
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
				if (ImGui::MenuItem("Rigidbody")) {
					if (!o->HasComponent<RigidbodyComponent>()) { o->AddComponent<RigidbodyComponent>(); }
				}
				if (ImGui::MenuItem("Box Collider")) {
					if (!o->HasComponent<BoxColliderComponent>()) { o->AddComponent<BoxColliderComponent>(); }
				}
				if (ImGui::MenuItem("Circle Collider")) {
					if (!o->HasComponent<CircleColliderComponent>()) { o->AddComponent<CircleColliderComponent>(); }
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

		float scaling[2] = { sr.scaling.x,sr.scaling.y };
		float offset [2] = { sr.offset.x, sr.offset.y };

		ImGui::Text("Scaling");
		ImGui::DragFloat2("##sr_scaling", scaling, 0.25f);

		ImGui::SameLine();

		if (ImGui::Button("=", { 28,28 })) {
			auto& tc = o->GetComponent<TransformComponent>();
			scaling[0] = tc.Scale.x;
			scaling[1] = tc.Scale.y;
		}

		ImGui::Text("Offset");
		ImGui::DragFloat2("##sr_offset", offset, 0.25f);

		sr.scaling.x = scaling[0];
		sr.scaling.y = scaling[1];
		sr.offset.x  = offset[0];
		sr.offset.y  = offset[1];

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

	

	void Panel_Inspector::DrawComponentRigidBody(Ref<Object> o) {
		ImGui::PushID("Rigidbody");

		RigidbodyComponent& rbc = o->GetComponent<RigidbodyComponent>();
		ImGui::Text("Rigidbody");
		ImGui::OpenPopupOnItemClick("RemoveComp");
		ImGui::NewLine();
		 
		const char* items[] = { "Static", "Dynamic", "Kinematic"};
		
		int curr_type = (int) rbc.Type;
		const char* current_item = items[curr_type];

		if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(items[n], is_selected)){
					switch (n) {
						case 0: rbc.Type = RigidbodyComponent::BodyType::Static; break;
						case 1: rbc.Type = RigidbodyComponent::BodyType::Dynamic; break;
						case 2: rbc.Type = RigidbodyComponent::BodyType::Kinematic; break;
						default: rbc.Type = RigidbodyComponent::BodyType::Static; break;
					}
					current_item = items[n];
				}
				if (is_selected){
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
				}
			}
			ImGui::EndCombo();
		}
		
		ImGui::NewLine();
		ImGui::Text("Fixed Rotation");
		ImGui::SameLine();
		ImGui::Checkbox("##fixedrotation", &rbc.FixedRotation);

		ImGui::Separator();

		if (ImGui::BeginPopup("RemoveComp")) {
			if (ImGui::Selectable("Remove")) {
				o->RemoveComponent<RigidbodyComponent>();
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	void Panel_Inspector::DrawComponentBoxCollider(Ref<Object> o) {
		ImGui::PushID("BoxCollider");
		BoxColliderComponent& bc = o->GetComponent<BoxColliderComponent>();
		ImGui::Text("Box Collider");
		ImGui::OpenPopupOnItemClick("RemoveComp");
		ImGui::NewLine();
		float size[2] = { bc.Size.x,bc.Size.y };
		float offset[2] = { bc.Offset.x, bc.Offset.y};

		ImGui::Text("Size");
		ImGui::DragFloat2("##size", size, 0.25f);

		ImGui::Text("Offset");
		ImGui::DragFloat2("##offset", offset, 0.25f);

		bc.Size = { size[0], size[1] };
		bc.Offset = { offset[0], offset[1] };

		DrawDragInputField("Material", &bc.physics_material_path, ".phys");

		ImGui::Separator();

		if (ImGui::BeginPopup("RemoveComp")) {
			if (ImGui::Selectable("Remove")) {
				o->RemoveComponent<BoxColliderComponent>();
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	void Panel_Inspector::DrawComponentCircleCollider(Ref<Object> o) {
		ImGui::PushID("CircleCollider");
		CircleColliderComponent& bc = o->GetComponent<CircleColliderComponent>();
		ImGui::Text("Circle Collider");
		ImGui::OpenPopupOnItemClick("RemoveComp");
		ImGui::NewLine();
		float radius = bc.Radius;
		float offset[2] = { bc.Offset.x, bc.Offset.y };

		ImGui::Text("Size");
		ImGui::DragFloat("##size", &radius, 0.25f);

		ImGui::Text("Offset");
		ImGui::DragFloat2("##offset", offset, 0.25f);

		bc.Radius = radius;
		bc.Offset = { offset[0], offset[1] };

		DrawDragInputField("Material", &bc.physics_material_path, ".phys");

		ImGui::Separator();

		if (ImGui::BeginPopup("RemoveComp")) {
			if (ImGui::Selectable("Remove")) {
				o->RemoveComponent<CircleColliderComponent>();
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

		DrawDragInputField("Script", &sc.path, ".cs");

		ImGui::NewLine();
		for (Script_Var& var : sc.variables) {

			ImGui::Text(var.name.c_str());
			ImGui::SameLine(ImGui::GetWindowWidth() - 225);


			char* nameBuffer = new char[var.user_value.length() + 16];
			std::strcpy(nameBuffer, var.user_value.c_str());
			std::string id = "##" + var.name;
			if (ImGui::InputText(id.c_str(), nameBuffer, var.user_value.length() + 16, ImGuiInputTextFlags_EnterReturnsTrue)) {
				var.isUserValueDefined = true;
				var.user_value = std::string(nameBuffer);
			}
			delete[] nameBuffer;

		}

		ImGui::Separator();

		if (ImGui::BeginPopup("RemoveComp")) {
			if (ImGui::Selectable("Remove")) {
				o->RemoveComponent<ScriptComponent>();
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}
}