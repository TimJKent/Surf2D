#pragma once
#include "mepch.h"
#include "TransformComponent.h"
#include "MechEngine/Renderer/Renderer2D.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace MechEngine {

	TransformComponent::TransformComponent(Object* object) : Component(object) {}
	void TransformComponent::Draw3DTransformUI(glm::vec3* position, std::string name, int defaultValue) {
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
	void TransformComponent::DrawUI() {
		glm::vec3 pos = transform.GetPosition();
		glm::vec3 rot = transform.GetRotation();
		glm::vec3 scale = transform.GetScale();

		ImGui::Text("Transform");
		Draw3DTransformUI(&pos, "Position   ", 0);
		Draw3DTransformUI(&rot, "Rotation   ", 0);
		Draw3DTransformUI(&scale, "Scale      ", 1);
		ImGui::NewLine();
		ImGui::Separator();

		transform.SetPosition(pos);
		transform.SetRotation(rot);
		transform.SetScale(scale);
	}
	void TransformComponent::OnUpdate()  {
	}

	void TransformComponent::Save() {
		if (!Serialization::ReadyForWrite()) {
			ME_ERROR("ERROR - ScreenMesh: Serializer not ready for Write");
			return;
		}
		Serialization::SERIAL_WRITE((int)transform.GetPosition().x);
		Serialization::SERIAL_WRITE((int)transform.GetPosition().y);
		Serialization::SERIAL_WRITE((int)transform.GetPosition().z);

		Serialization::SERIAL_WRITE((int)transform.GetRotation().x);
		Serialization::SERIAL_WRITE((int)transform.GetRotation().y);
		Serialization::SERIAL_WRITE((int)transform.GetRotation().z);
		
		Serialization::SERIAL_WRITE((int)transform.GetScale().x);
		Serialization::SERIAL_WRITE((int)transform.GetScale().y);
		Serialization::SERIAL_WRITE((int)transform.GetScale().z);
	}

	void TransformComponent::Load() {
		int x, y, z = 0;
		Serialization::SERIAL_READ(&x);
		Serialization::SERIAL_READ(&y);
		Serialization::SERIAL_READ(&z);

		transform.SetPosition({ x,y,z });
							
		Serialization::SERIAL_READ(&x);;
		Serialization::SERIAL_READ(&y);;
		Serialization::SERIAL_READ(&z);;

		transform.SetRotation({ x,y,z });
		
		Serialization::SERIAL_READ(&x);
		Serialization::SERIAL_READ(&y);
		Serialization::SERIAL_READ(&z);

		transform.SetScale({ x,y,z });
	}

	void TransformComponent::OnDelete() {
		
	}
}