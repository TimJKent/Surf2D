#include "SurfEngine.h"
#include "SurfEngine/Core/EntryPoint.h"

#include "Panels/Project.h"
#include "Panels/ProjectManager.h"
#include "Panels/Panel_Hierarchy.h"
#include "Panels/Panel_Inspector.h"
#include "Panels/Panel_Viewport.h"
#include "Panels/Panel_AssetBrowser.h"

#include "imgui/imgui.h"
#include "imgui/imconfig.h"
#include "imgui/imgui_internal.h"

#include <glfw/include/GLFW/glfw3.h>

#include "stb_image/stb_image.h"

namespace SurfEngine {

	static bool viewport_is_selected;
	static Ref<OrthographicCamera> sceneCamera;

	class EditorLayer : public Layer {
	public:
		EditorLayer() :
			Layer("Editor")

		{}

		void OnAttach() override {
			Project::InitProjectsDirectory();
			m_panel_hierarchy = std::make_shared<Panel_Hierarchy>();
			m_panel_inspector = std::make_shared<Panel_Inspector>(m_panel_hierarchy);
			m_panel_viewport = std::make_shared<Panel_Viewport>();
			m_panel_assetbrowser = std::make_shared<Panel_AssetBrowser>(Project::GetProjectsDirectory());
			ProjectManager::SetWindowTitle();
			SetWindowIcon();
		}

		void OnImGuiRender() override {
			static char* buff = new char[50];

			//Initilize Dockspace
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.7f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
			if (ImGui::Begin("DockSpace", &m_GuiIsActive, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
			{
				ImGui::SetWindowSize({ (float)Application::Get().GetWindow().GetWidth(), (float)Application::Get().GetWindow().GetHeight() - 20 });
				ImGui::SetWindowPos({ (float)Application::Get().GetWindow().GetPosX(), (float)Application::Get().GetWindow().GetPosY() + 20 });
			}
			if (m_GuiIsActive)
			{
				ImGui::BeginMainMenuBar();
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
							ImGui::InputText("", buff,50);
							if (ImGui::Button("Create", ImVec2(120, 0))) { CreateProject(std::string(buff)); ImGui::CloseCurrentPopup(); }
							ImGui::SetItemDefaultFocus();
							ImGui::SameLine();
							if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
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
							ImGui::InputText("", buff, 50);
							if (ImGui::Button("Create", ImVec2(120, 0))) { NewScene(std::string(buff)); ImGui::CloseCurrentPopup(); }
							ImGui::SetItemDefaultFocus();
							ImGui::SameLine();
							if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
							ImGui::EndPopup();
						}
						ImGui::EndMenu();
					}
					if (ImGui::MenuItem("Save Scene", "", false , ProjectManager::IsActiveProject())) { SaveScene(); }

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("GameObject", ProjectManager::IsActiveScene())) {
					if (ImGui::MenuItem("Add GameObject")) {
						Object square = ProjectManager::GetActiveScene()->CreateObject();
					}
					ImGui::Separator();
					Object o = m_panel_hierarchy->GetSelectedObject();
					if (ImGui::BeginMenu("Add Component", o)) {
						if (ImGui::MenuItem("Sprite Renderer")) {
							if (!o.HasComponent<SpriteRendererComponent>()) { o.AddComponent<SpriteRendererComponent>(); }
						}
						ImGui::EndMenu();
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Tools")) {
					if (ImGui::MenuItem("Debug Mode",NULL,m_panel_inspector->GetDebugMode())) {
						m_panel_inspector->SetDebugMode(!m_panel_inspector->GetDebugMode());
					}
					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();

				

				
				

				//Declare Central dockspace
				m_DockspaceId = ImGui::GetID("HUB_DockSpace");
				ImGui::DockSpace(m_DockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode/*|ImGuiDockNodeFlags_NoResize*/);
			}
			ImGui::End();






			m_panel_viewport->OnImGuiRender();
			m_panel_hierarchy->OnImGuiRender();
			m_panel_inspector->OnImGuiRender();
			m_panel_assetbrowser->OnImGuiRender();



			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			viewport_is_selected = m_panel_viewport->GetSelected();
		
		}

	private:
		Ref<Panel_Hierarchy> m_panel_hierarchy;
		Ref<Panel_Inspector> m_panel_inspector;
		Ref<Panel_Viewport> m_panel_viewport;
		Ref<Panel_AssetBrowser> m_panel_assetbrowser;

		bool  m_GuiIsActive = true;
		ImGuiID m_DockspaceId = 0;
	private:
		void OpenScene(const std::string& filepath) {
			Ref<Scene> openedScene = std::make_shared<Scene>();
			SceneSerializer serializer(openedScene);
			serializer.Deserialze(filepath);
			ProjectManager::SetActiveScene(openedScene);
		}

		void NewScene(const std::string& name) {
			Ref<Scene> newScene = std::make_shared<Scene>();
			newScene->SetName(name);
			ProjectManager::SetActiveScene(newScene);
			SaveScene();
		}

		void SaveScene() {
			SceneSerializer serializer(ProjectManager::GetActiveScene());
			serializer.Serialze(ProjectManager::GetActiveProject()->GetProjectDirectory()+"\\"+ ProjectManager::GetActiveScene()->GetName() +".scene");
		}


		void CreateProject(std::string name) {
			Ref<Project> project = std::make_shared<Project>(name);
			ProjectManager::SetActiveProject(project);
			m_panel_assetbrowser->SetPath(ProjectManager::GetActiveProject()->GetProjectDirectory());
			m_panel_assetbrowser->SetHighestPath(ProjectManager::GetActiveProject()->GetProjectDirectory());
		}

		

		void SetWindowIcon() {
			int width, height, channels;
			//stbi_set_flip_vertically_on_load(1);
			stbi_uc* data = nullptr;
			{
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

	class RenderLoopLayer : public Layer {

	public:
		RenderLoopLayer() : Layer("Example") {

		}

		void OnAttach() override {

			//Enable Textures, Important for FBO to work correctly - probably should be moved
			RenderCommand::EnableTextures();
			RenderCommand::EnableBlending();
			RenderCommand::EnableDepthTesting();
			RenderCommand::EnableMSAA();
			Renderer2D::Init();

			sceneCamera.reset(new OrthographicCamera());
			
			sceneCamera->SetZoom(4.0f);

				
			//Create the FrameBuffer
			FramebufferSpecification fbSpec;
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA8 , FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
			fbSpec.Width = 1920;
			fbSpec.Height = 1080;
			Renderer2D::SetRenderTarget(Framebuffer::Create(fbSpec));
		
			
		}

		void OnUpdate(Timestep timestep) override {
			//UpdateObjects
			if (viewport_is_selected && ProjectManager::IsActiveScene()) {
				sceneCamera->OnUpdate(timestep);

			}

			//RenderScene
			if (ProjectManager::IsActiveScene()) {
				Renderer2D::BeginScene(sceneCamera);
				Renderer2D::DrawBackgroundGrid(1);
				ProjectManager::GetActiveScene()->OnUpdate(timestep);
				Renderer2D::EndScene();
			}
		}

		void OnEvent(Event& event) override {
			if (viewport_is_selected) {
				sceneCamera->OnEvent(event);
			}
		}

	private:
		bool mouseCheck = true;

	};

	class EngineEditor : public Application {
	public:
		EngineEditor() {
			PushLayer(new EditorLayer());
			PushLayer(new RenderLoopLayer());
		}

		~EngineEditor() {}
	};

	Application* CreateApplication() {
		return new EngineEditor();
	}

}
