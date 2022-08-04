#include "Panel_AssetBrowser.h"

#include "SurfEngine.h"
#include "../Util/ProjectManager.h"
#include "../Util/Project.h"

#include <filesystem>

#include "glm/glm.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "../Util/Resource.h"


namespace SurfEngine {

	Panel_AssetBrowser::Panel_AssetBrowser(const std::string& root_path) 
	{
		Resource::InitIcons();
	}

	void Panel_AssetBrowser::OnRefresh() {
		image_asset_icons.clear();
		file_count = 0;
		for (auto& p : std::filesystem::directory_iterator(ProjectManager::GetPath())) {
			file_count++;
			if(Resource::GetResourceType(p.path()) == Resource::IMAGE) {
				image_asset_icons.push_back(Texture2D::Create(p.path().string()));
			}
		}
	}

	void Panel_AssetBrowser::OnImGuiRender() {
		if (ImGui::Begin("Asset Browser",NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration)) {
			
			UpdateFileCount();
			DoMenuPopupContext();
			DrawParentPathText();
			DrawBackButton();

			int panel_width = (int)ImGui::GetContentRegionAvail().x;
			int img_size = 96;
			int img_padding = 20;

			
			if ((panel_width / (img_size + img_padding)) > 2) {
				ImGui::Columns((panel_width) / (img_size + img_padding), NULL, false);
				DrawFileIcons(img_size);
				DrawDirectoryIcons(img_size);
			}
			
			ImGui::End();
		}
	}

	void Panel_AssetBrowser::UpdateFileCount() {
		std::size_t file_counter = 0;
		for (auto& p : std::filesystem::directory_iterator(ProjectManager::GetPath())) {
			file_counter++;
		}
		if (file_count != file_counter) {
			OnRefresh();
		}
	}

	void Panel_AssetBrowser::DrawParentPathText() {
		std::size_t pathlength = ProjectManager::GetPath().length();
		std::size_t loc = ProjectManager::GetPath().find("\\SurfEngine", 0);
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
		if (ProjectManager::GetPath().compare(ProjectManager::GetHighestPath()) == 0) { return; }
	
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 30);
		if (ImGui::Button("Back")) {
			std::filesystem::path path(ProjectManager::GetPath());
			ProjectManager::SetPath(path.parent_path().string());
		}
	}

	void Panel_AssetBrowser::DrawFileIcons(int icon_size) {
		unsigned int img_icon_counter = 0;
		for (auto& p : std::filesystem::directory_iterator(ProjectManager::GetPath())) {
			if (p.is_directory()) { continue;}
			
			std::string id = p.path().filename().string();
			Resource::ResourceType rtype = Resource::GetResourceType(p.path());
			Ref<Texture2D> icon = Resource::GetResourceIcon(rtype);

			if (ProjectManager::IsActiveProject() && (rtype == Resource::PROJECT)) { continue; }
			if (!ProjectManager::IsActiveProject() && (rtype != Resource::PROJECT)) { continue; }
			if (rtype == Resource::IMAGE) { icon = image_asset_icons[img_icon_counter++]; }

			ImGui::PushID(id.c_str());
			ImGui::BeginGroup();
			ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2((float)icon_size, (float)icon_size), ImVec2(0, 1), ImVec2(1, 0));

			if (ImGui::IsItemHovered())
			{
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					ProjectManager::OpenFileInEditor(p);
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

			if (ImGui::BeginPopupContextItem((id + "popup").c_str()))
			{
				if (ImGui::MenuItem("Open")) {
					ProjectManager::OpenFileInEditor(p);
				}
				if (ImGui::MenuItem("Duplicate")) {
					ProjectManager::DuplicateFile(p);
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Delete")) {
					ProjectManager::s_SelectedPath = "";
					ProjectManager::DeleteFileA(p);
				}
				ImGui::EndPopup();
			}
		}
	}

	void Panel_AssetBrowser::DrawDirectoryIcons(int icon_size) {
		for (auto& p : std::filesystem::directory_iterator(ProjectManager::GetPath())) {
			if (p.is_directory()) {
				ImGui::PushID(p.path().filename().string().c_str());
				ImGui::BeginGroup();
				ImGui::PushItemFlag(ImGuiButtonFlags_PressedOnDoubleClick, true);
				ImGui::ImageButton((ImTextureID)(uint64_t)Resource::GetResourceIcon(Resource::DIRECTORY)->GetRendererID(), ImVec2((float)icon_size, (float)icon_size), ImVec2(0, 1), ImVec2(1, 0));

				if (ImGui::IsItemHovered())
				{
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
						ProjectManager::SetPath(p.path().string());
						OnRefresh();
					}
				}

				if (ImGui::BeginDragDropSource()) {
					ImGui::Image((ImTextureID)(uint64_t)Resource::GetResourceIcon(Resource::DIRECTORY)->GetRendererID(), ImVec2(32.f, 32.f), ImVec2(0, 1), ImVec2(1, 0));
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

	void Panel_AssetBrowser::DoMenuPopupContext() {
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
	}

}
