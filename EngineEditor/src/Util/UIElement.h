#pragma once
#define SE_DEFAULT_INPUT_TEXT_PADDING 16
#include <string>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/glm.hpp>

#include "SurfEngine/Renderer/Texture.h"

namespace SurfEngine {
	namespace UIElement {

		struct TexturePathPair {
			Ref<Texture2D> texture;
			std::string path;
		};

		std::string InputText(const std::string& label, std::string& string_ref, const std::string& extra_id = "") {
			size_t nameBufferSize = string_ref.size() + SE_DEFAULT_INPUT_TEXT_PADDING;
			char* string_ref_buffer = new char[nameBufferSize];

			std::strcpy(string_ref_buffer, string_ref.c_str());

			ImGui::Text(label.c_str());
			ImGui::SameLine();

			std::string id = label + extra_id;
			ImGui::PushID(id.c_str());
			ImGui::InputText("", string_ref_buffer, nameBufferSize, ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopID();
			delete[] string_ref_buffer;

			return std::string(string_ref_buffer);
		}

		void InputTextReadOnly(const std::string& label, std::string& string_ref, const std::string& extra_id = "") {
			size_t nameBufferSize = string_ref.size() + SE_DEFAULT_INPUT_TEXT_PADDING;
			char* string_ref_buffer = new char[nameBufferSize];

			std::strcpy(string_ref_buffer, string_ref.c_str());

			ImGui::Text(label.c_str());
			ImGui::SameLine();

			std::string id = label + extra_id;
			ImGui::PushID(id.c_str());
			ImGui::InputText("", string_ref_buffer, nameBufferSize, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_ReadOnly);
			ImGui::PopID();
			delete[] string_ref_buffer;
		}

		int InputInt(const std::string& label, int vector, const std::string& extra_id = "") {
			ImGui::Text(label.c_str());
			ImGui::SameLine();

			std::string id = "##" + label + extra_id;
			int int_vec[1] = { vector };
			ImGui::InputInt(id.c_str(), int_vec, 1);

			return { int_vec[0] };
		}

		float InputFloat(const std::string& label, float vector, const std::string& extra_id = "") {
			ImGui::Text(label.c_str());
			ImGui::SameLine();

			std::string id = "##" + label + extra_id;
			float float_vec[1] = { vector};
			ImGui::DragFloat(id.c_str(), float_vec, 0.25f);

			return { float_vec[0]};
		}

		glm::vec2 InputFloat2(const std::string& label, glm::vec2 vector, const std::string& extra_id = "") {
			ImGui::Text(label.c_str());
			ImGui::SameLine();

			std::string id = "##" + label + extra_id;
			float float_vec[2] = { vector.x, vector.y };
			ImGui::DragFloat2(id.c_str(), float_vec, 0.25f);

			return { float_vec[0], float_vec[1] };
		}

		glm::vec3 InputFloat3(const std::string& label, glm::vec3 vector, const std::string& extra_id = "") {
			ImGui::Text(label.c_str());
			ImGui::SameLine();

			std::string id = "##" + label + extra_id;
			float float_vec[3] = { vector.x, vector.y, vector.z };
			ImGui::DragFloat3(id.c_str(), float_vec, 0.25f);

			return { float_vec[0], float_vec[1], float_vec[2] };
		}

		void DragInputField(const std::string& label, std::string* path_ref, const std::string& extension) {
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

		UIElement::TexturePathPair DragInputTexture(const std::string& label, UIElement::TexturePathPair texture, const std::string& extension) {
			UIElement::TexturePathPair texture_output = texture;

			ImGui::Text(label.c_str());
			std::string id = label + extension;
			ImGui::PushID(id.c_str());

			void* imageId = texture.texture ? (ImTextureID)(uint64_t)texture.texture->GetRendererID() : nullptr;
			ImGui::Image(imageId, ImVec2{ 128,128 }, ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));

			if (ImGui::BeginDragDropTarget())
			{
				char* path;
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("fileitem");
				if (payload) {
					path = new char[payload->DataSize + 1];
					memcpy((char*)&path[0], payload->Data, payload->DataSize);
					texture_output.path = path;
					texture_output.texture = Texture2D::Create(texture_output.path);
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::PopID();

			
			std::string btn_label = "Select " + label;
			if (ImGui::Button(btn_label.c_str())) {
				std::string img_path = FileDialogs::OpenFile(ProjectManager::GetPath(), "Image (*.png)\0*.png\0");
				if (!img_path.empty()) {
					texture_output.path = img_path;
					texture_output.texture = Texture2D::Create(texture_output.path);
				}
			}
			ImGui::SameLine();
			std::string btn_label_remove = "Clear " + label;
			if (ImGui::Button(btn_label_remove.c_str())) {
				texture_output.path = "";
				texture_output.texture = nullptr;
			}


			return texture_output;
		}

		glm::vec3 InputColorRGB(const std::string& label, glm::vec3 color, const std::string& extra_id = "") {
			float color_vec[3] = { color.r,color.g, color.b};
			ImGui::Text(label.c_str());
			ImGui::ColorEdit3("", color_vec);
			return { color_vec[0],color_vec[1], color_vec[2]};
		}

		glm::vec4 InputColorRGBA(const std::string& label, glm::vec4 color, const std::string& extra_id = "") {
			float color_vec[4] = { color.r,color.g, color.b, color.a };
			ImGui::Text(label.c_str());
			ImGui::ColorEdit4("", color_vec);
			return { color_vec[0],color_vec[1], color_vec[2], color_vec[3] };
		}

		void BeginRemovable(const std::string& id) {
			ImGui::PushID(id.c_str());
			ImGui::OpenPopupOnItemClick("RemoveComp");
		}

		int InputSelectable(const std::string& label, std::vector<std::string> items, int current_item) {
			if (ImGui::BeginCombo("##combo", items[current_item].c_str()))
			{
				int n = 0;
				for (; n < items.size(); n++)
				{
					bool is_selected = (current_item == n);
					if (ImGui::Selectable(items[n].c_str(), is_selected)) {
						current_item = n;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			return current_item;
		}

		template<typename T>
		void EndRemovable(std::vector<Ref<Object>> objects) {
			if (ImGui::BeginPopup("RemoveComp")) {
				if (ImGui::Selectable("Remove")) {
					for (int i = 0; i < objects.size(); i++) {
						objects[i]->RemoveComponent<T>();
					}
				}
				ImGui::EndPopup();
			}
			ImGui::PopID();
		}
	}
}