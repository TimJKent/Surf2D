#include "Panel_AssetBrowser.h"

#include "SurfEngine.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "glm/glm.hpp"


#include <filesystem>

namespace SurfEngine {

	std::string s_AssetDirectory = "C:\\Users\\timbe\\source\\repos\\SurfEngine";

	Panel_AssetBrowser::Panel_AssetBrowser() {
		m_DirectoryIcon = Texture2D::Create("res\\textures\\icon_folder.png");
		m_FileIcon = Texture2D::Create("res\\textures\\icon_file.png");
		m_ImageIcon = Texture2D::Create("res\\textures\\icon_image.png");
	}


	void Panel_AssetBrowser::OnImGuiRender() {

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f,0.2f,0.2f,1.0f));
		if(ImGui::Begin("Asset Browser")){
			if (ImGui::Button("Back")) {
				std::filesystem::path path(s_AssetDirectory);
				s_AssetDirectory = path.parent_path().string();
			}
			float panel_width = ImGui::GetContentRegionAvail().x;
			int img_size = 96.f;
			int img_padding = 16.f;
			
			ImGui::Columns((panel_width) / (img_size+img_padding), NULL,false);
			for (auto& p : std::filesystem::directory_iterator(s_AssetDirectory)) {
				if (p.is_directory()) {
					ImGui::BeginGroup();
					ImGui::PushItemFlag(ImGuiButtonFlags_PressedOnDoubleClick,true);
					ImGui::ImageButton((ImTextureID)m_DirectoryIcon->GetRendererID(), ImVec2(img_size, img_size));

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						s_AssetDirectory = p.path().string();;

					}

					ImGui::PopItemFlag();
					ImGui::Text(p.path().filename().string().c_str());
					ImGui::EndGroup();
					ImGui::NextColumn();
				}
			}
			for (auto& p : std::filesystem::directory_iterator(s_AssetDirectory)) {
				if (!p.is_directory()) {
					ImGui::BeginGroup();
					Ref<Texture2D> icon = m_FileIcon; 
					if (p.path().extension().string().compare(".png") == 0 || p.path().extension().string().compare(".jpg") == 0) {
						icon = m_ImageIcon;
					}

					if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(img_size, img_size))) {
					}
					if (ImGui::BeginDragDropSource())
					{
						ImGui::Image((ImTextureID)icon->GetRendererID(), ImVec2(32, 32));
						ImGui::SameLine();
						ImGui::Text(p.path().filename().string().c_str());
						const wchar_t* itemPath = p.path().c_str();
						ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
						ImGui::EndDragDropSource();
					}
					ImGui::Text(p.path().filename().string().c_str());
					ImGui::EndGroup();
					ImGui::NextColumn();
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleColor();

	}
}
