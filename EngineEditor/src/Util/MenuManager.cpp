#include "MenuManager.h"
#include "AssetSerializer.h"
#include "SurfEngine/Physics/PhysicsEngine.h"
#include "SurfEngine/Scripting/ScriptEngine.h"

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

void DrawGeneralOptions() {
	ImGui::Text("General Options");
}

void DrawInputOptions() {
	ImGui::Text("Input Options");
}

void DrawRendererOptions() {
	ImGui::Text("Renderer Options");
}

void DrawPhysicsOptions() {

	float gravity_scale[2] = {
		PhysicsEngine::s_Data.gravity_scale.x,
		PhysicsEngine::s_Data.gravity_scale.y
	};
	ImGui::InputFloat2("Gravity Scale", gravity_scale,"%.1f");
	PhysicsEngine::s_Data.gravity_scale = {gravity_scale[0], gravity_scale[1]};

	int32_t velocity_iterations = PhysicsEngine::s_Data.velocity_iterations;
	ImGui::InputInt("Velocity Iterations", &velocity_iterations, 1, 1);
	PhysicsEngine::s_Data.velocity_iterations = (uint32_t)abs(velocity_iterations);

	int32_t position_iterations = PhysicsEngine::s_Data.position_iterations;
	ImGui::InputInt("Position Iterations", &position_iterations, 1, 1);
	PhysicsEngine::s_Data.position_iterations = (uint32_t)abs(position_iterations);
}

void DrawScriptingOptions() {
	ImGui::Text("Scripting Options");
	
	int buffersize = ScriptEngine::s_Data->csc_path.size();
	char* textBuffer = new char[buffersize+15];
	std::strcpy(textBuffer, ScriptEngine::s_Data->csc_path.c_str());

	ImGui::InputText("C Sharp Compiler Path", textBuffer, buffersize+15);

	ScriptEngine::s_Data->csc_path = textBuffer;
}

void MenuManager::DrawProjectPropertiesPopup() {
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextItemWidth(400);
	
	static bool physicsSelected = true;
	static bool rendererSelected = false;

	if (ImGui::BeginPopupModal("Project Properties", NULL))
	{
		ImGui::BeginColumns("#prop_divider, 2",ImGuiColumnsFlags_NoResize);
		ImGui::SetColumnWidth(0, 120);
		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		const std::string MenuItems[5] = {
			"General",
			"Input",
			"Rendering",
			"Physics",
			"Scripting",
		};

		static int node_clicked = 0;
		for (int i = 0; i < 5; i++)
		{
			// Disable the default "open on single-click behavior" + set Selected flag according to our selection.
			ImGuiTreeNodeFlags node_flags = base_flags;
			const bool is_selected = node_clicked == i;
			if (is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;
			
			// Items 3..5 are Tree Leaves
			// The only reason we use TreeNode at all is to allow selection of the leaf. Otherwise we can
			// use BulletText() or advance the cursor by GetTreeNodeToLabelSpacing() and call Text().
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
			ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, MenuItems[i].c_str(), i);
			if (ImGui::IsItemClicked())
				node_clicked = i;

		}
		ImGui::NextColumn();

		switch (node_clicked) {
			case 0:	DrawGeneralOptions();	break;
			case 1:	DrawInputOptions();		break;
			case 2:	DrawRendererOptions();	break;
			case 3:	DrawPhysicsOptions();	break;
			case 4:	DrawScriptingOptions();	break;
		}
		
		ImGui::EndColumns();
		ImGui::Separator();
		ImGui::NewLine();
		ImGui::NewLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - 100);
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 48);
		if (ImGui::Button("Apply", { 100,32 })) {
			AssetSerializer::SaveProjectProperties(ProjectManager::GetActiveProject()->GetName());
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Exit", { 100,32})) {
			ImGui::CloseCurrentPopup();
		}
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
		ProjectManager::LoadProject(filepath);
}