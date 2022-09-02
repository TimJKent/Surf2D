#include "sepch.h"

#include "Panel_Inspector.h"
#include "SurfEngine/Scenes/Components.h"
#include "SurfEngine/Renderer/Renderer2D.h"
#include "SurfEngine/Core/PlatformUtils.h"
#include "SurfEngine/Physics/PhysicsEngine.h"
#include "../Util/ProjectManager.h"
#include "../Util/SceneManager.h"
#include "../Util/UIElement.h"

#include <filesystem>
#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace SurfEngine {
	void Panel_Inspector::OnImGuiRender() {
		if (ImGui::Begin("Inspector")) {
			if (SceneManager::IsSelectedObject()) {
				if (SceneManager::HasComponenetForAllSelected<TagComponent>()) { DrawComponentTag(SceneManager::GetSelectedObject()); }
				if (SceneManager::HasComponenetForAllSelected<TransformComponent>()) { DrawComponentTransform(SceneManager::GetSelectedObject()); }
				if (SceneManager::HasComponenetForAllSelected<SpriteRendererComponent>()) { DrawComponentSpriteRenderer(SceneManager::GetSelectedObject()); }
				if (SceneManager::HasComponenetForAllSelected<AnimationComponent>()) { DrawComponentAnimation(SceneManager::GetSelectedObject()); }
				if (SceneManager::HasComponenetForAllSelected<CameraComponent>()) { DrawComponentCamera(SceneManager::GetSelectedObject()); }
				if (SceneManager::HasComponenetForAllSelected<RigidbodyComponent>()) { DrawComponentRigidBody(SceneManager::GetSelectedObject()); }
				if (SceneManager::HasComponenetForAllSelected<BoxColliderComponent>()) { DrawComponentBoxCollider(SceneManager::GetSelectedObject()); }
				if (SceneManager::HasComponenetForAllSelected<CircleColliderComponent>()) { DrawComponentCircleCollider(SceneManager::GetSelectedObject()); }
				if (SceneManager::HasComponenetForAllSelected<ScriptComponent>()) { DrawComponentScript(SceneManager::GetSelectedObject()); }
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
				if (ImGui::MenuItem("Sprite Renderer")) {
					SceneManager::AddComponentToSelected<SpriteRendererComponent>();
				}
				if (ImGui::MenuItem("Animation")) {
					SceneManager::AddComponentToSelected<AnimationComponent>();
				}
				if (ImGui::MenuItem("Camera")) {
					SceneManager::AddComponentToSelected<CameraComponent>();
				}
				if (ImGui::MenuItem("Script")) {
					SceneManager::AddComponentToSelected<ScriptComponent>();
				}
				if (ImGui::MenuItem("Rigidbody")) {
					SceneManager::AddComponentToSelected<RigidbodyComponent>();
				}
				if (ImGui::MenuItem("Box Collider")) {
					SceneManager::AddComponentToSelected<BoxColliderComponent>();
				}
				if (ImGui::MenuItem("Circle Collider")) {
					SceneManager::AddComponentToSelected<CircleColliderComponent>();
				}
				ImGui::EndPopup();
		}
		ImGui::End();
	}

	void Panel_Inspector::DrawComponentTag(std::vector<Ref<Object>> objects) {
		//Draw Name
		if (objects.size() > 1) {
			std::string multi_obj_str = "    " + std::to_string(objects.size()) + " Objects Selected";
			ImGui::Text(multi_obj_str.c_str());
		}
		else{
			TagComponent& tc = objects[0]->GetComponent<TagComponent>();
			std::string name = UIElement::InputText("Name", tc.Tag);
			if (!name.empty()) { tc.Tag = name; }
		}

		//Draw UUID
		if (ProjectManager::DebugModeOn) {
			for (int i = 0; i < objects.size(); i++) {
				TagComponent& tc = objects[i]->GetComponent<TagComponent>();
				UIElement::InputTextReadOnly("UUID", tc.uuid.ToString(), std::to_string(i));
			}
		}

		ImGui::Separator();
	}

	void Panel_Inspector::DrawComponentTransform(std::vector<Ref<Object>> objects) {
		ImGui::Text("Transform");
		ImGui::NewLine();

		if (objects.size() == 1) {
			TransformComponent& tc = objects[0]->GetComponent<TransformComponent>();
			glm::vec2 newTranslation = UIElement::InputFloat2("Translation", { tc.Translation.x, -tc.Translation.y });
			tc.Translation	= { newTranslation.x, -newTranslation.y};
			tc.Rotation		= UIElement::InputFloat ("Rotation",	  tc.Rotation);
			tc.Scale		= UIElement::InputFloat2("Scale", { tc.Scale.x, tc.Scale.y });
		}
		else {
			 glm::vec2	deltaTranslation = UIElement::InputFloat2("Translation", { 0.f, 0.f });
			 float		deltaRotation = UIElement::InputFloat("Rotation", 0.f);
			 glm::vec2	deltaScale = UIElement::InputFloat2("Scale", {0.0f, 0.0f});

			for (int i = 0; i < objects.size(); i++) {
				auto& tc = objects[i]->GetComponent<TransformComponent>();
				tc.Translation.x += deltaTranslation.x;
				tc.Translation.y += -deltaTranslation.y;
				tc.Scale.x += deltaScale.x;
				tc.Scale.y += deltaScale.y;
				tc.Rotation += deltaRotation;
			}
		}
		
		ImGui::Separator();
	}

	void Panel_Inspector::DrawComponentSpriteRenderer(std::vector<Ref<Object>> objects) {
		ImGui::Text("Sprite Renderer");
		UIElement::BeginRemovable("SpriteComp");

		SpriteRendererComponent& sr = objects[0]->GetComponent<SpriteRendererComponent>();
		ImGui::NewLine();

		sr.Color = UIElement::InputColorRGBA("Base Color", sr.Color);
		ImGui::NewLine();

		int layer_value = UIElement::InputInt("Layer", sr.Layer);
		sr.Layer = layer_value < 0 ? sr.Layer : layer_value;
		ImGui::NewLine();

		UIElement::TexturePathPair tpp = UIElement::DragInputTexture("Sprite", { sr.Texture, sr.Texture_Path}, "SpriteSelector");
		sr.Texture = tpp.texture;
		sr.Texture_Path = tpp.path;
		ImGui::SameLine();
		ImGui::Checkbox("Flip X", &sr.flipX);
		ImGui::NewLine();

		sr.scaling = UIElement::InputFloat2("Scaling", sr.scaling, "sr");
		ImGui::SameLine();

		if (ImGui::Button("=", { 28,28 })) {
			auto& tc = objects[0]->GetComponent<TransformComponent>();
			sr.scaling = { tc.Scale.x, tc.Scale.y };
		}

		sr.offset = UIElement::InputFloat2("Offset", sr.scaling, "sr");

		ImGui::Separator();

		UIElement::EndRemovable<SpriteRendererComponent>(objects);
	}

	void Panel_Inspector::DrawComponentCamera(std::vector<Ref<Object>> objects) {

		ImGui::Text("Camera");
		UIElement::BeginRemovable("CameraComp");

		CameraComponent& cc = objects[0]->GetComponent<CameraComponent>();
		auto& camera = cc.Camera;
		float csize = cc.Camera.GetOrthographicSize();
		float cnear = cc.Camera.GetOrthographicNearClip();
		float cfar  = cc.Camera.GetOrthographicFarClip();
		
		camera.SetOrthographicSize(UIElement::InputFloat("Size", cc.Camera.GetOrthographicSize()));
		camera.SetOrthographicNearClip(UIElement::InputFloat("Near Clip ", cc.Camera.GetOrthographicNearClip()));
		camera.SetOrthographicFarClip(UIElement::InputFloat("Far Clip ", cc.Camera.GetOrthographicFarClip()));


		ImGui::Separator();

		UIElement::EndRemovable<CameraComponent>(objects);
	}

	void Panel_Inspector::DrawComponentAnimation(std::vector<Ref<Object>> objects) {

		ImGui::Text("Animation");
		UIElement::BeginRemovable("AnimationComp");

		AnimationComponent& ac = objects[0]->GetComponent<AnimationComponent>();

		ImGui::NewLine();

		ac.frames = UIElement::InputInt("Frames", ac.frames);
		ac.currframe = UIElement::InputInt("Current Frame", ac.currframe);
		ac.fps = UIElement::InputInt("Fps", ac.fps);

		ImGui::Checkbox("PlayOnAwake", &ac.playOnAwake);
		ImGui::Checkbox("Loop", &ac.loop);

		ImGui::Separator();

		UIElement::EndRemovable<AnimationComponent>(objects);
	}

	

	void Panel_Inspector::DrawComponentRigidBody(std::vector<Ref<Object>> objects) {

		ImGui::Text("Rigidbody");
		UIElement::BeginRemovable("RigidbodyComp");

		RigidbodyComponent& rbc = objects[0]->GetComponent<RigidbodyComponent>();
		ImGui::NewLine();
		 
		std::vector<std::string> items = { "Static", "Dynamic", "Kinematic" };
		rbc.Type = (RigidbodyComponent::BodyType) UIElement::InputSelectable("Simulation Type", items, (int)rbc.Type);
		
		ImGui::NewLine();
		ImGui::Text("Fixed Rotation");
		ImGui::SameLine();
		ImGui::Checkbox("##fixedrotation", &rbc.FixedRotation);

		ImGui::Separator();

		UIElement::EndRemovable<RigidbodyComponent>(objects);
	}

	void Panel_Inspector::DrawComponentBoxCollider(std::vector<Ref<Object>> objects) {
		
		ImGui::Text("Box Collider");
		UIElement::BeginRemovable("BoxcolComp");
		ImGui::NewLine();

		BoxColliderComponent& bc = objects[0]->GetComponent<BoxColliderComponent>();

		bc.Size = UIElement::InputFloat2("Size", bc.Size, "phys");
		bc.Offset = UIElement::InputFloat2("Offset", bc.Offset, "phys");
		ImGui::Checkbox("Sensor", &bc.isSensor);
		UIElement::DragInputField("Material", &bc.physics_material_path, ".phys");

		ImGui::Separator();

		UIElement::EndRemovable<BoxColliderComponent>(objects);
	}

	void Panel_Inspector::DrawComponentCircleCollider(std::vector<Ref<Object>> objects) {
		ImGui::Text("Circle Collider");
		UIElement::BeginRemovable("CirclecolComp");
		ImGui::NewLine();

		CircleColliderComponent& cc = objects[0]->GetComponent<CircleColliderComponent>();

		cc.Radius = UIElement::InputFloat("Radius", cc.Radius, "phys");
		cc.Offset = UIElement::InputFloat2("Offset", cc.Offset, "phys");
		ImGui::Checkbox("Sensor", &cc.isSensor);
		UIElement::DragInputField("Material", &cc.physics_material_path, ".phys");

		ImGui::Separator();

		UIElement::EndRemovable<CircleColliderComponent>(objects);
	}

	void Panel_Inspector::DrawComponentScript(std::vector<Ref<Object>> objects) {

		ImGui::Text("Script");
		UIElement::BeginRemovable("ScriptComp");

		ScriptComponent& sc = objects[0]->GetComponent<ScriptComponent>();

		ImGui::NewLine();

		UIElement::DragInputField("Script", &sc.path, ".cs");

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

		UIElement::EndRemovable<ScriptComponent>(objects);
	}
}