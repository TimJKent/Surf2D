#include "Panel_AssetBrowser.h"

#include "SurfEngine.h"
#include "../Util/ProjectManager.h"
#include "../Util/Project.h"

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
		m_ScriptIcon = Texture2D::Create("res\\textures\\icon_script.png");
		m_ProjectIcon = Texture2D::Create("res\\textures\\icon_project.png");
	}

	void Panel_AssetBrowser::OnRefresh() {
		image_asset_icons.clear();
		file_count = 0;
		for (auto& p : std::filesystem::directory_iterator(ProjectManager::GetPath())) {
			file_count++;
			if (p.path().extension().string().compare(".png") == 0 || p.path().extension().string().compare(".jpg") == 0) {
				image_asset_icons.push_back(Texture2D::Create(p.path().string()));
			}
		}
	}

	void Panel_AssetBrowser::OnImGuiRender() {
		if (ImGui::Begin("Asset Browser",NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration)) {
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Open In Explorer")) {
					FileDialogs::OpenExplorer(ProjectManager::GetPath());
				}
				if (ImGui::MenuItem("New Folder")) {
					ProjectManager::CreateFolder(ProjectManager::GetPath(), "New Folder");
				}
				ImGui::EndPopup();
			}

			UpdateFileCount();
			DrawParentPathText();
			DrawBackButton();

			int panel_width = (int)ImGui::GetContentRegionAvail().x;
			int img_size = 96;
			int img_padding = 20;

			
			if (((panel_width) / (img_size + img_padding)) > 2) {
				ImGui::Columns((panel_width) / (img_size + img_padding), NULL, false);
				DrawFileIcons(96);
				DrawDirectoryIcons(96);
			}
			

			ImGui::End();
		}
	}

	void Panel_AssetBrowser::UpdateFileCount() {
		unsigned int file_counter = 0;
		for (auto& p : std::filesystem::directory_iterator(ProjectManager::GetPath())) {
			file_counter++;
		}
		if (file_count != file_counter) {
			OnRefresh();
		}
	}

	void Panel_AssetBrowser::DrawParentPathText() {
		int pathlength = (int)ProjectManager::GetPath().length();
		int loc = (int)ProjectManager::GetPath().find("\\SurfEngine", 0);
		std::string parent_path = ProjectManager::GetPath();
		ImGui::PushID("movetoparent");
		ImGui::Text((".." + parent_path.substr(loc, pathlength)).c_str());
		if (ProjectManager::GetPath().compare(ProjectManager::GetHighestPath()) != 0) {
			if (ImGui::BeginDragDropTarget())
			{
				char* path;
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("fileitem");
				if (payload) {
					path = new char[payload->DataSize + 1];
					memcpy((char*)&path[0], payload->Data, payload->DataSize);
					std::filesystem::path file_path = path;
					std::filesystem::path file_copy = parent_path;
					std::filesystem::copy(file_path, file_copy.parent_path() / file_path.filename(), std::filesystem::copy_options::recursive);
					std::filesystem::remove_all(file_path);
				}
				ImGui::EndDragDropTarget();
			}
		}
		ImGui::PopID();
	}

	void Panel_AssetBrowser::DrawBackButton() {
		if (ProjectManager::GetPath().compare(ProjectManager::GetHighestPath()) != 0) {
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 30);
			if (ImGui::Button("Back")) {
				std::filesystem::path path(ProjectManager::GetPath());
				ProjectManager::SetPath(path.parent_path().string());
				OnRefresh();
			}
		}
	}

	void Panel_AssetBrowser::DrawFileIcons(int icon_size) {
		unsigned int img_icon_counter = 0;
		for (auto& p : std::filesystem::directory_iterator(ProjectManager::GetPath())) {
			if (!p.is_directory()) {
				ImGui::PushID(p.path().filename().string().c_str());
				ImGui::BeginGroup();
				Ref<Texture2D> icon = m_FileIcon;

				if (p.path().extension().string().compare(".png") == 0 || p.path().extension().string().compare(".jpg") == 0) {
					icon = image_asset_icons[img_icon_counter++];
				}
				if (p.path().extension().string().compare(".scene") == 0) {
					icon = m_SceneIcon;
				}
				if (p.path().extension().string().compare(".cs") == 0) {
					icon = m_ScriptIcon;
				}
				if (p.path().extension().string().compare(".surf") == 0) {
					icon = m_ProjectIcon;
				}

				ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2((float)icon_size, (float)icon_size), ImVec2(0, 1), ImVec2(1, 0));

				if (ImGui::IsItemHovered())
				{
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
						if (p.path().extension().string().compare(".scene") == 0) {
							ProjectManager::OpenScene(p.path().string());
						}
						else if (p.path().extension().string().compare(".surf") == 0) {
							ProjectManager::OpenProject(p.path().string());
						}
						else {
							system(p.path().string().c_str());
						}
					}
					else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
						ProjectManager::SetSelectedPath(p.path().string());
					}
				}

				if (ImGui::BeginDragDropSource()) {
					ImGui::Image((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(32.f, 32.f), ImVec2(0, 1), ImVec2(1, 0));
					ImGui::SameLine();
					ImGui::Text(p.path().filename().string().c_str());
					std::string path = p.path().string().c_str();
					path += '\0';
					ImGui::SetDragDropPayload("fileitem", path.c_str(), path.length());
					ImGui::EndDragDropSource();
				}

				ImGui::Text(p.path().filename().string().c_str());
				ImGui::EndGroup();
				ImGui::NextColumn();
				ImGui::PopID();
			}
		}
	}

	void Panel_AssetBrowser::DrawDirectoryIcons(int icon_size) {
		for (auto& p : std::filesystem::directory_iterator(ProjectManager::GetPath())) {
			if (p.is_directory()) {
				ImGui::PushID(p.path().filename().string().c_str());
				ImGui::BeginGroup();
				ImGui::PushItemFlag(ImGuiButtonFlags_PressedOnDoubleClick, true);
				ImGui::ImageButton((ImTextureID)(uint64_t)m_DirectoryIcon->GetRendererID(), ImVec2((float)icon_size, (float)icon_size), ImVec2(0, 1), ImVec2(1, 0));

				if (ImGui::IsItemHovered())
				{
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
						ProjectManager::SetPath(p.path().string());
						OnRefresh();
					}
				
				}

				if (ImGui::BeginDragDropSource()) {
					ImGui::Image((ImTextureID)(uint64_t)m_DirectoryIcon->GetRendererID(), ImVec2(32.f, 32.f), ImVec2(0, 1), ImVec2(1, 0));
					ImGui::SameLine();
					ImGui::Text(p.path().filename().string().c_str());
					std::string path = p.path().string();
					path += '\0';
					ImGui::SetDragDropPayload("fileitem", path.c_str(), path.length());
					ImGui::EndDragDropSource();
				}

				if (ImGui::BeginDragDropTarget())
				{
					char* path;
					const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("fileitem");
					if (payload) {
						path = new char[payload->DataSize + 1];
						memcpy((char*)&path[0], payload->Data, payload->DataSize);
						std::filesystem::path file_path = path;
						std::filesystem::copy(file_path, p.path() / file_path.filename(), std::filesystem::copy_options::recursive);
						std::filesystem::remove_all(file_path);
					}

					ImGui::EndDragDropTarget();
				}


				ImGui::PopItemFlag();
				ImGui::Text(p.path().filename().string().c_str());
				ImGui::EndGroup();
				ImGui::NextColumn();
				ImGui::PopID();
			}
		}
	}
}
