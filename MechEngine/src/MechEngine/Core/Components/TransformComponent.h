#pragma once
#include "MechEngine/Core/Object.h"
#include "MechEngine/Core/Components/Component.h"
#include "MechEngine/Core/Transform.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace MechEngine {
	

	class TransformComponenet : public Component {
	public:
		TransformComponenet() {}
		void Draw3DTransformUI(glm::vec3* position, std::string name, int defaultValue) {
			ImGui::PushID(name.c_str());
			ImGui::Text(name.c_str());
			ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });
			ImVec2 buttonSize(19.0f, 19.0f);
			int x;
			int y;
			int z;
			x = position->x;
			y = position->y;
			z = position->z;
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.7f,0.0f,0.0f,1.0f });
			if (ImGui::Button("X", buttonSize)) {
				x = defaultValue;
			}
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(40.f);
			ImGui::DragInt("##X", &x, 1.0f);
			ImGui::PopStyleVar();


			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f,0.7f,0.0f,1.0f });
			if (ImGui::Button("Y", buttonSize)) {
				y = defaultValue;

			}
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(40.f);
			ImGui::DragInt("##Y", &y, 1.0f);
			ImGui::PopStyleVar();

			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f,0.0f,0.7f,1.0f });
			if (ImGui::Button("Z", buttonSize)) {
				z = defaultValue;

			}
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(40.f);
			ImGui::DragInt("##Z", &z, 1.0f);
			ImGui::PopStyleVar();

			position->x = x;
			position->y = y;
			position->z = z;
			ImGui::PopID();
		}
		void DrawUI() override {
			glm::vec3 pos = transform.GetPosition();
			glm::vec3 rot = transform.GetRotation();
			glm::vec3 scale = transform.GetScale();

			ImGui::Text("Transform");
			Draw3DTransformUI(&pos, "Position   ", 0);
			Draw3DTransformUI(&rot, "Rotation   ", 0);
			Draw3DTransformUI(&scale, "Scale      ", 1);
			ImGui::Separator();

			transform.SetPosition(pos);
			transform.SetRotation(rot);
			transform.SetScale(scale);
		}
		void OnUpdate() override {}
	public:
		Transform transform;
	};
}