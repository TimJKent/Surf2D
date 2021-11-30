#include "Panel_AssetBrowser.h"

#include "SurfEngine.h"
#include "ProjectManager.h"
#include "Project.h"

#include <filesystem>

#include "glm/glm.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>



namespace SurfEngine {

	Panel_AssetBrowser::Panel_AssetBrowser(const std::string& root_path) 
	{
		m_DirectoryIcon = Texture2D::Create("res\\textures\\icon_folder.png");
		m_FileIcon = Texture2D::Create("res\\textures\\icon_file.png");
		m_ImageIcon = Texture2D::Create("res\\textures\\icon_image.png");
		m_SceneIcon = Texture2D::Create("res\\textures\\icon_globe.png");
	}


	void Panel_AssetBrowser::OnImGuiRender() {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f,0.2f,0.2f,1.0f));
		if(ImGui::Begin("Asset Browser")){
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Open In Explorer")) {
					FileDialogs::OpenExplorer(ProjectManager::GetPath());
				}
				ImGui::EndPopup();
			}
			int pathlength = ProjectManager::GetPath().length();
			int loc = ProjectManager::GetPath().find("\\SurfEngine", 0);
			std::string path = ProjectManager::GetPath();
				path = path.substr(loc, pathlength);
			ImGui::Text(path.c_str());
			if (ProjectManager::GetPath().compare(ProjectManager::GetHighestPath()) != 0) {
				ImGui::SameLine(ImGui::GetContentRegionAvail().x - 30);
				if (ImGui::Button("Back")) {
					std::filesystem::path path(ProjectManager::GetPath());
					ProjectManager::SetPath(path.parent_path().string());
				}
			}

			int panel_width = (int)ImGui::GetContentRegionAvail().x;
			int img_size = 96;
			int img_padding = 16;
			
			ImGui::Columns((panel_width) / (img_size+img_padding), NULL,false);
			for (auto& p : std::filesystem::directory_iterator(ProjectManager::GetPath())) {
				if (p.is_directory()) {
					ImGui::BeginGroup();
					ImGui::PushItemFlag(ImGuiButtonFlags_PressedOnDoubleClick,true);
					ImGui::ImageButton((ImTextureID)(uint64_t)m_DirectoryIcon->GetRendererID(), ImVec2((float)img_size, (float)img_size), ImVec2(0, 1), ImVec2(1, 0));

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						ProjectManager::SetPath(p.path().string());
					}


					ImGui::PopItemFlag();
					ImGui::Text(p.path().filename().string().c_str());
					ImGui::EndGroup();
					ImGui::NextColumn();
				}
			}
			
			for (auto& p : std::filesystem::directory_iterator(ProjectManager::GetPath())) {
				if (!p.is_directory()) {
					ImGui::BeginGroup();
					Ref<Texture2D> icon = m_FileIcon; 
					if (p.path().extension().string().compare(".png") == 0 || p.path().extension().string().compare(".jpg") == 0) {
						icon = m_ImageIcon;
					}
					if (p.path().extension().string().compare(".scene") == 0 ) {
						
						icon = m_SceneIcon;
					}

					ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2((float)img_size, (float)img_size), ImVec2(0, 1), ImVec2(1, 0));
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						if (p.path().extension().string().compare(".scene") == 0) {
							ProjectManager::OpenScene(p.path().string());
						}
						if (p.path().extension().string().compare(".surf") == 0) {
							ProjectManager::OpenProject(p.path().string());
						}
					}
					if (ImGui::BeginDragDropSource())
					{
						ImGui::Image((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(32.f, 32.f), ImVec2(0, 1), ImVec2(1, 0));
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
