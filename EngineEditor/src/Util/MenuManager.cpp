#include "MenuManager.h"

static char* input_buff;

void MenuManager::Init() {
	input_buff = new char[50];
	input_buff[0] = '\0';
}

void MenuManager::DrawNewProjectPopup() {
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("New Project Creation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Create New Project\n\n");
		ImGui::Separator();
		ImGui::Text("Name: ");
		ImGui::SameLine();
		ImGui::InputText("", input_buff, 50);
		if (ImGui::Button("Create", ImVec2(120, 0))) { ProjectManager::CreateProject(std::string(input_buff)); ImGui::CloseCurrentPopup(); input_buff[0] = '\0'; }
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); input_buff[0] = '\0'; }
		ImGui::EndPopup();
	}
}

void MenuManager::DrawNewScenePopup() {
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("New Scene Creation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Create New Scene\n\n");
		ImGui::Separator();
		ImGui::Text("Name: ");
		ImGui::SameLine();
		ImGui::InputText("", input_buff, 50);
		if (ImGui::Button("Create", ImVec2(120, 0))) { NewScene(std::string(input_buff)); ImGui::CloseCurrentPopup(); input_buff[0] = '\0'; }
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); input_buff[0] = '\0'; }
		ImGui::EndPopup();
	}
}

void MenuManager::BeginDialogue_OpenProject() {
	std::string filepath = FileDialogs::OpenFile(ProjectManager::GetProjectsDirectory(), "Surf Project (*.surf)\0*.surf\0");
	SE_CORE_INFO(filepath);

	if (!filepath.empty())
		ProjectManager::OpenProject(filepath);
}