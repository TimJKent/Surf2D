#pragma once

#include "SurfEngine.h"
#include "../Util/Project.h"
#include "../Util/ProjectManager.h"
#include "../Panels/Panel_Hierarchy.h"
#include "../Panels/Panel_Inspector.h"
#include "../Panels/Panel_Viewport.h"
#include "../Panels/Panel_AssetBrowser.h"


#include "imgui/imgui.h"
#include "imgui/imconfig.h"
#include "imgui/imgui_internal.h"

#include <glfw/include/GLFW/glfw3.h>

#include "stb_image/stb_image.h"

using namespace SurfEngine;

class EditorLayer : public Layer {
public:
	EditorLayer(EngineLayer& runtime) :
		Layer("Editor"), m_runtime(runtime)

	{}

	void OnAttach() override {

		ProjectManager::InitProjectsDirectory();
		m_panel_hierarchy = std::make_shared<Panel_Hierarchy>();
		m_panel_inspector = std::make_shared<Panel_Inspector>(m_panel_hierarchy);
		m_panel_viewport = std::make_shared<Panel_Viewport>();
		m_panel_assetbrowser = std::make_shared<Panel_AssetBrowser>(ProjectManager::GetProjectsDirectory());
		ProjectManager::SetWindowTitle();
		SetWindowIcon();
		input_buff[0] = '\0';
		{
			ImGuiStyle& style = ImGui::GetStyle();

			style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.94f);
			style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.32f, 0.32f, 0.32f, 0.50f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.47f, 0.47f, 0.47f, 0.67f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.23f, 0.23f, 0.23f, 0.67f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.03f, 0.03f, 0.03f, 0.53f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.11f, 0.13f, 0.23f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.17f, 0.20f, 0.34f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.24f, 0.28f, 0.48f, 1.00f);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.46f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.28f, 0.48f, 1.00f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.28f, 0.32f, 0.53f, 1.00f);
			style.Colors[ImGuiCol_Button] = ImVec4(0.23f, 0.23f, 0.23f, 0.40f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
			style.Colors[ImGuiCol_Separator] = ImVec4(0.53f, 0.53f, 0.53f, 0.50f);
			style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.69f, 0.69f, 0.69f, 0.78f);
			style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.24f, 0.28f, 0.48f, 1.00f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.30f, 0.51f, 1.00f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.31f, 0.35f, 0.57f, 1.00f);
			style.Colors[ImGuiCol_Tab] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
			style.Colors[ImGuiCol_TabHovered] = ImVec4(0.34f, 0.34f, 0.34f, 0.73f);
			style.Colors[ImGuiCol_TabActive] = ImVec4(0.19f, 0.19f, 0.19f, 0.73f);
			style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.31f, 0.31f, 0.31f, 0.97f);
			style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
			style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.60f, 0.60f, 0.60f, 0.70f);
			style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
			style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
			style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
			style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
			style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
			style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
			style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
			style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
			style.FrameBorderSize = 1.0f;
			style.WindowBorderSize = 1.0f;
			style.PopupBorderSize = 1.0f;
			style.FramePadding = { 6.0f, 6.0f };
			style.ItemInnerSpacing = { 3.0f, 4.0f };
			style.ScrollbarSize = 12;
			style.GrabMinSize = 12;
			style.WindowRounding = 10;
			style.ChildRounding = 0;
			style.FrameRounding = 2;
			style.PopupRounding = 10;
			style.ScrollbarRounding = 10;
			style.TabRounding = 6;
			style.WindowTitleAlign = { 0.5f,0.5f };
			style.WindowMenuButtonPosition = ImGuiDir_None;
		}
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("res\\OpenSans-VariableFont.ttf", 16);
	}

	void OnImGuiRender() override {
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

			static auto first_time = true;
			if (first_time)
			{
				first_time = false;

				ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
				ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
				ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

				// split the dockspace into 2 nodes -- DockBuilderSplitNode takes in the following args in the following order
				//   window ID to split, direction, fraction (between 0 and 1), the final two setting let's us choose which id we want (which ever one we DON'T set as NULL, will be returned by the function)
				//                                                              out_id_at_dir is the id of the node in the direction we specified earlier, out_id_at_opposite_dir is in the opposite direction
				auto dock_id_inspector = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);
				auto dock_id_assetbrowser = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);
				auto dock_id_hiearchy = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);
				ImGuiDockNode* Node = ImGui::DockBuilderGetNode(dockspace_id);
				Node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;


				// we now dock our windows into the docking node we made above
				ImGui::DockBuilderDockWindow("Hierarchy", dock_id_hiearchy);
				ImGui::DockBuilderDockWindow("Inspector", dock_id_inspector);
				ImGui::DockBuilderDockWindow("View Port", dockspace_id);
				ImGui::DockBuilderDockWindow("Asset Browser", dock_id_assetbrowser);
				ImGui::DockBuilderFinish(dockspace_id);
			}
		}

		ImGui::End();




		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::BeginMenu("New")) {
					if (ImGui::Button("New Project"))
						ImGui::OpenPopup("New Project Creation");


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

					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !ProjectManager::IsActiveProject());
					if (ImGui::Button("New Scene"))
						ImGui::OpenPopup("New Scene Creation");
					ImGui::PopItemFlag();

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
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Save Scene", "", false, ProjectManager::IsActiveProject())) { SaveScene(); }
				if (ImGui::MenuItem("Open Project")) { BeginDialogue_OpenProject(); }

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Assets", ProjectManager::IsActiveProject())) {
				if (ImGui::MenuItem("Add Script")) {
					std::string precode = "";
					precode += "function OnStart()\n";
					precode += "\n";
					precode += "end\n";
					precode += "\n";
					precode += "function OnUpdate(TimeStep)\n";
					precode += "\n";
					precode += "end\n";
					precode += "\n";
					precode += "function OnEnd()\n";
					precode += "";
					precode += "end\n";

					ProjectManager::CreateFileA(ProjectManager::GetPath(), "script", ".lua");
					ProjectManager::WriteInFileA(ProjectManager::GetPath() + "\\script.lua", precode);

				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("GameObject", ProjectManager::IsActiveScene())) {
				if (ImGui::MenuItem("Add GameObject")) {
					ProjectManager::GetActiveScene()->CreateObject();
				}
				ImGui::Separator();
				Ref<Object> o = ProjectManager::GetSelectedObject();
				if (ImGui::BeginMenu("Add Component", o.get())) {
					if (ImGui::MenuItem("Sprite Renderer")) {
						if (!o->HasComponent<SpriteRendererComponent>()) { o->AddComponent<SpriteRendererComponent>(); }
					}
					if (ImGui::MenuItem("Animation")) {
						if (!o->HasComponent<AnimationComponent>()) { o->AddComponent<AnimationComponent>(); }
					}
					if (ImGui::MenuItem("Camera")) {
						if (!o->HasComponent<CameraComponent>()) { o->AddComponent<CameraComponent>(); }
					}
					if (ImGui::MenuItem("Script")) {
						if (!o->HasComponent<LuaScriptComponent>()) { o->AddComponent<LuaScriptComponent>(); }
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}

			static bool DrawDemo = false;
			if (ImGui::BeginMenu("Tools")) {
				if (ImGui::MenuItem("Debug Mode", NULL, m_panel_inspector->GetDebugMode())) {
					m_panel_inspector->SetDebugMode(!m_panel_inspector->GetDebugMode());
				}

				if (ImGui::MenuItem("ImGuiDemoMode", nullptr, &DrawDemo)) {

				}
				if (ImGui::MenuItem("Draw Grid", NULL, &s_draw_grid)) {
				}
				ImGui::EndMenu();
			}
			if (DrawDemo)
				ImGui::ShowDemoWindow();
			ImGui::EndMainMenuBar();

		}
		m_panel_hierarchy->OnImGuiRender();
		m_panel_inspector->OnImGuiRender();
		m_panel_assetbrowser->OnImGuiRender();
		m_panel_viewport->OnImGuiRender();
		m_runtime.UpdateCamera = m_panel_viewport->GetSelected();
	}

private:
	Ref<Panel_Hierarchy> m_panel_hierarchy;
	Ref<Panel_Inspector> m_panel_inspector;
	Ref<Panel_Viewport> m_panel_viewport;
	Ref<Panel_AssetBrowser> m_panel_assetbrowser;

	EngineLayer& m_runtime;

	char* input_buff = new char[50];
	bool  m_GuiIsActive = true;
	ImGuiID m_DockspaceId = 0;
	bool s_draw_grid;


private:

	void OpenScene(const std::string& filepath) {
		ProjectManager::OpenScene(filepath);
	}

	void NewScene(const std::string& name) {
		ProjectManager::NewScene(name);
	}

	void SaveScene() {
		ProjectManager::SaveCurrentScene();
	}

	void BeginDialogue_OpenProject() {
		std::string filepath = FileDialogs::OpenFile(ProjectManager::GetProjectsDirectory(), "Surf Project (*.surf)\0*.surf\0");
		SE_CORE_INFO(filepath);

		if (!filepath.empty())
			ProjectManager::OpenProject(filepath);
	}

	void SetWindowIcon() {
		int width, height, channels;
		//stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			stbi_set_flip_vertically_on_load(0);
			data = stbi_load("res\\textures\\window_icon.png", &width, &height, &channels, 0);
		}

		GLFWimage image;
		image.pixels = data;
		image.height = height;
		image.width = width;

		GLFWwindow* window = (GLFWwindow*)SurfEngine::Application::Get().GetWindow().GetNativeWindow();
		glfwSetWindowIcon(window, 1, &image);
	}
};