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
		: m_RootPath(root_path), m_HighestDirectory(root_path)
	{
		m_DirectoryIcon = Texture2D::Create("res\\textures\\icon_folder.png");
		m_FileIcon = Texture2D::Create("res\\textures\\icon_file.png");
		m_ImageIcon = Texture2D::Create("res\\textures\\icon_image.png");
		m_SceneIcon = Texture2D::Create("res\\textures\\icon_globe.png");
	}


	void Panel_AssetBrowser::OnImGuiRender() {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f,0.2f,0.2f,1.0f));
		if(ImGui::Begin("Asset Browser")){
			ImGui::Text(m_RootPath.substr(m_RootPath.find("\\SurfEngine", 0), m_RootPath.length()).c_str());
			if (m_RootPath.compare(m_HighestDirectory) != 0) {
				ImGui::SameLine(ImGui::GetContentRegionAvail().x - 30);
				if (ImGui::Button("Back")) {
					std::filesystem::path path(m_RootPath);
					m_RootPath = path.parent_path().string();
				}
			}

			int panel_width = (int)ImGui::GetContentRegionAvail().x;
			int img_size = 96;
			int img_padding = 16;
			
			ImGui::Columns((panel_width) / (img_size+img_padding), NULL,false);
			for (auto& p : std::filesystem::directory_iterator(m_RootPath)) {
				if (p.is_directory()) {
					ImGui::BeginGroup();
					ImGui::PushItemFlag(ImGuiButtonFlags_PressedOnDoubleClick,true);
					ImGui::ImageButton((ImTextureID)(uint64_t)m_DirectoryIcon->GetRendererID(), ImVec2((float)img_size, (float)img_size));

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						if (ProjectManager::IsActiveProject()) {
							m_RootPath = p.path().string();
						}
						else {
							Ref<Project> project = std::make_shared<Project>();
							project->SetName(p.path().filename().string());
							project->SetProjectDirectory(p.path().string());
							ProjectManager::SetActiveProject(project);
							m_RootPath = p.path().string();
							m_HighestDirectory = m_RootPath;
						}
					}


					ImGui::PopItemFlag();
					ImGui::Text(p.path().filename().string().c_str());
					ImGui::EndGroup();
					ImGui::NextColumn();
				}
			}
			for (auto& p : std::filesystem::directory_iterator(m_RootPath)) {
				if (!p.is_directory()) {
					ImGui::BeginGroup();
					Ref<Texture2D> icon = m_FileIcon; 
					if (p.path().extension().string().compare(".png") == 0 || p.path().extension().string().compare(".jpg") == 0) {
						icon = m_ImageIcon;
					}
					if (p.path().extension().string().compare(".scene") == 0 ) {
						
						icon = m_SceneIcon;
					}

					ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2((float)img_size, (float)img_size));
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						if (p.path().extension().string().compare(".scene") == 0) {
							Ref<Scene> openedScene = std::make_shared<Scene>();
							SceneSerializer serializer(openedScene);
							serializer.Deserialze(p.path().string());
							ProjectManager::SetActiveScene(openedScene);
						}
					}
					if (ImGui::BeginDragDropSource())
					{
						ImGui::Image((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(32.f, 32.f));
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

	void Panel_AssetBrowser::SetPath(const std::string& path) {
		m_RootPath = path;
	}

	void Panel_AssetBrowser::SetHighestPath(const std::string& path) {
		m_HighestDirectory = path;
	}
}
