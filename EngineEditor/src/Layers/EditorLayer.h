#pragma once
#include "SurfEngine.h"

#include "../Util/Project.h"
#include "../Util/ProjectManager.h"
#include "../Util/FileManager.h"
#include "../Util/SceneManager.h"
#include "../Panels/Panel_Hierarchy.h"
#include "../Panels/Panel_Inspector.h"
#include "../Panels/Panel_Viewport.h"
#include "../Panels/Panel_AssetBrowser.h"
#include "../Util/MenuManager.h"
#include "../Util/AssetSerializer.h"
#include "EngineLayer.h"
#include "imgui/imgui.h"
#include "imgui/imconfig.h"
#include "imgui/imgui_internal.h"

#include "stb_image/stb_image.h"

using namespace SurfEngine;

class EditorLayer : public Layer {

public:
	EditorLayer(EngineLayer& runtime) :
		Layer("Editor"), m_runtime(runtime)
	{}

	void OnAttach() override {
	    MenuManager::Init();
		ProjectManager::InitProjectsDirectory();
		m_panel_hierarchy = std::make_shared<Panel_Hierarchy>();
		m_panel_inspector = std::make_shared<Panel_Inspector>(m_panel_hierarchy);
		m_panel_viewport = std::make_shared<Panel_Viewport>();
		m_panel_assetbrowser = std::make_shared<Panel_AssetBrowser>(ProjectManager::GetProjectsDirectory());
		ProjectManager::SetWindowTitle();
		InitWindowIcon();
		InitIOSettings();
	}

	void OnImGuiRender() override {
		static bool is_doing_command = false;

		m_panel_inspector->SetDebugMode(ProjectManager::DebugModeOn);

		//TODO: Move these somewhere else
		if (Input::IsKeyPressed(SE_KEY_LEFT_CONTROL) || Input::IsKeyPressed(SE_KEY_RIGHT_CONTROL)) {
			if (Input::IsKeyPressed(SE_KEY_A) && !is_doing_command) {
				is_doing_command = true;
				SceneManager::PushAllObjectsToSelection();
			}
			if (Input::IsKeyPressed(SE_KEY_D) && !is_doing_command) {
				is_doing_command = true;
				SceneManager::DuplicateSelected();
			}
		}
		if (Input::IsKeyPressed(SE_KEY_DELETE) && !is_doing_command) {
			is_doing_command = true;
			SceneManager::DeleteSelected();
		}
		if (Input::IsKeyPressed(SE_KEY_ESCAPE) && !is_doing_command) {
			is_doing_command = true;
			SceneManager::ClearSelectedObject();
		}

		if (!Input::IsKeyPressed(SE_KEY_A) && !Input::IsKeyPressed(SE_KEY_D) && !Input::IsKeyPressed(SE_KEY_DELETE) && !Input::IsKeyPressed(SE_KEY_ESCAPE)) {
			is_doing_command = false;
		}

		DrawDockSpace();
		DrawMenuBar();

		m_panel_hierarchy->OnImGuiRender();
		m_panel_inspector->OnImGuiRender();
		m_panel_assetbrowser->OnImGuiRender();
		m_panel_viewport->OnImGuiRender();
		m_runtime.settings.UpdateCamera = m_panel_viewport->GetSelected();
	}

private:
	Ref<Panel_Hierarchy> m_panel_hierarchy;
	Ref<Panel_Inspector> m_panel_inspector;
	Ref<Panel_Viewport> m_panel_viewport;
	Ref<Panel_AssetBrowser> m_panel_assetbrowser;

	EngineLayer& m_runtime;

	ImGuiID m_DockspaceId = 0;

private:

	void DrawDockSpace() {
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
	}

	void DrawMenuBar() {
		if (ImGui::BeginMainMenuBar()) {
			DrawFileMenu();
			DrawProjectMenu();
			DrawSceneMenu();
			DrawGameObjectMenu();

			ImGui::EndMainMenuBar();
		}
	}


	void DrawFileMenu() {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::BeginMenu("New")) {
				if (ImGui::Button("New Project")) { 
					ImGui::OpenPopup("New Project Creation");
				}
				
				
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !ProjectManager::IsActiveProject());
				if (ImGui::Button("New Scene")) { ImGui::OpenPopup("New Scene Creation"); }
				ImGui::PopItemFlag();

				MenuManager::DrawNewProjectPopup();
				MenuManager::DrawNewScenePopup();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Open")) {
				if (ImGui::MenuItem("Open Project")) { MenuManager::BeginDialogue_OpenProject(); }
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Save", "", false, SceneManager::IsActiveScene())) { MenuManager::SaveScene(); }
			
			
			ImGui::Separator();

			if (ImGui::MenuItem("Exit")) {
			
			};
			ImGui::EndMenu();
		}
	}

	void CreateAsset_Script() {
		std::string precode = "";
		precode += "using SurfEngine;\n";
		precode += "\n";
		precode += "class new_script : Script\n";
		precode += "{\n";
		precode += "	public void OnStart()\n";
		precode += "	{\n";
		precode += "\n";
		precode += "	}\n";
		precode += "\n";
		precode += "	public void OnUpdate()\n";
		precode += "	{\n";
		precode += "\n";
		precode += "	}\n";
		precode += "}\n";

		FileManager::CreateFileA(ProjectManager::GetPath(), "new_script", ".cs");
		FileManager::WriteInFileA(ProjectManager::GetPath() + "\\new_script.cs", precode);
	}

	 void CreateAsset_PhysicsMaterial() {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Physics Material";
		out << YAML::BeginMap; // TransformComponent
		out << YAML::Key << "Density" << YAML::Value << 1.0f;
		out << YAML::Key << "Friction" << YAML::Value << 0.5f;
		out << YAML::Key << "Restitution" << YAML::Value << 0.0f;
		out << YAML::Key << "RestitutionThreshold" << YAML::Value << 0.5f;
		out << YAML::EndMap;
		out << YAML::EndMap;

		std::string path = ProjectManager::GetPath();
		path += "\\phys_material.phys";

		std::ofstream fout(path);
		fout << out.c_str();
		fout.close();
	}

	

	void DrawProjectMenu() {
		if (ImGui::BeginMenu("Project", ProjectManager::IsActiveProject())) {
			if (ImGui::BeginMenu("Create Asset")) {
				if (ImGui::MenuItem("Script")) {
					CreateAsset_Script();
				}
				if (ImGui::MenuItem("Physics Material")) {
					CreateAsset_PhysicsMaterial();
				}
				ImGui::EndMenu();
			}
			
			if (ImGui::MenuItem("Show In Explorer")) {
				FileDialogs::OpenExplorer(ProjectManager::GetHighestPath());
			}

			ImGui::Separator();
			if (ImGui::Button("Properties")) {
				
				ImGui::OpenPopup("Project Properties");

				MenuManager::DrawProjectPropertiesPopup();

			}
			MenuManager::DrawProjectPropertiesPopup();


			ImGui::EndMenu();
		}

	}

	void DrawSceneMenu() {
		if (ImGui::BeginMenu("Scene", SceneManager::IsActiveScene())) {
			if (ImGui::MenuItem("Add GameObject")) {
				auto& scene = SceneManager::GetActiveScene();
				scene->CreateObject("New GameObject");
			}
			ImGui::EndMenu();
		}
	}

	void DrawGameObjectMenu() {
		if (ImGui::BeginMenu("GameObject", SceneManager::IsSelectedObject())) {
			auto& o = SceneManager::GetSelectedObject()[0];
			if (ImGui::BeginMenu("Add Component", o.get())) {
				if (ImGui::MenuItem("Sprite Renderer")) {
					SceneManager::AddComponentToSelected<SpriteRendererComponent>();
				}
				if (ImGui::MenuItem("Animation")) {
					SceneManager::AddComponentToSelected<AnimationComponent>();
				}
				if (ImGui::MenuItem("Camera")) {
					SceneManager::AddComponentToSelected<CameraComponent>();
				}
				if (ImGui::MenuItem("Rigidbody")) {
					SceneManager::AddComponentToSelected<RigidbodyComponent>();
				}
				if (ImGui::MenuItem("Box Collider")) {
					SceneManager::AddComponentToSelected<BoxColliderComponent>();
				}
				if (ImGui::MenuItem("Circle Collider")) {
					SceneManager::AddComponentToSelected<CircleColliderComponent>();
				}
				if (ImGui::MenuItem("Script")) {
					SceneManager::AddComponentToSelected<ScriptComponent>();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
	}


	
	void InitWindowIcon() {
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

	void InitIOSettings() {
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
};