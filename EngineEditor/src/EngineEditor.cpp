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


#include "CameraController.h"

namespace SurfEngine {

	static bool viewport_is_selected;
	static bool s_draw_grid = true;
	static Ref<OrthographicCamera> s_EditorCamera;
	static bool s_play = false;
	

	class EditorLayer : public Layer {
	public:
		EditorLayer() :
			Layer("Editor")

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
			m_PlayButton_PlayIcon = Texture2D::Create("res\\textures\\icon_play.png");
			m_PlayButton_StopIcon = Texture2D::Create("res\\textures\\icon_stop.png");
			m_PlayButton_CurrIcon = m_PlayButton_PlayIcon;
		}

		void OnImGuiRender() override {
			
			//Initilize Dockspace
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.7f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			
			if (true)
			{
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			}

			ImGui::Begin("DockSpace", &m_GuiIsActive, window_flags);
			ImGui::PopStyleVar(2);
			
			if(ImGui::BeginMainMenuBar()){
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
							if (ImGui::Button("Create", ImVec2(120, 0))) { ProjectManager::CreateProject(std::string(input_buff)); ImGui::CloseCurrentPopup(); input_buff[0] = '\0';}
							ImGui::SetItemDefaultFocus();
							ImGui::SameLine();
							if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); input_buff[0] = '\0';}
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
							if (ImGui::Button("Create", ImVec2(120, 0))) { NewScene(std::string(input_buff)); ImGui::CloseCurrentPopup(); input_buff[0] = '\0';}
							ImGui::SetItemDefaultFocus();
							ImGui::SameLine();
							if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); input_buff[0] = '\0';}
							ImGui::EndPopup();
						}
						ImGui::EndMenu();
					}
					if (ImGui::MenuItem("Save Scene", "", false, ProjectManager::IsActiveProject())) { SaveScene(); }
					if (ImGui::MenuItem("Open Project")) { BeginDialogue_OpenProject(); }

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
						if (ImGui::MenuItem("Camera")) {
							if (!o.HasComponent<CameraComponent>()) { o.AddComponent<CameraComponent>(); }
						}
						if (ImGui::MenuItem("Script")) {
							if (!o.HasComponent<NativeScriptComponent>()) { o.AddComponent<NativeScriptComponent>().Bind<CameraController>(); }
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

				

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
				

				

				//Declare Central dockspace
				m_DockspaceId = ImGui::GetID("HUB_DockSpace");
				ImGui::DockSpace(m_DockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode/*|ImGuiDockNodeFlags_NoResize*/);
			}
			ImGui::End();



			if (ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
				float size = ImGui::GetWindowHeight() - 4.0f;
				ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
				int img_size = 24;
				if(ImGui::ImageButton((ImTextureID)(uint64_t)m_PlayButton_CurrIcon->GetRendererID(), ImVec2((float)img_size, (float)img_size), ImVec2(0, 1), ImVec2(1, 0))) {
					s_play = !s_play;
					if (s_play) {
						m_PlayButton_CurrIcon = m_PlayButton_StopIcon;
					}
					else {
						m_PlayButton_CurrIcon = m_PlayButton_PlayIcon;
					}
				}
			}
			ImGui::PopStyleVar(2);
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
		Ref<Texture2D> m_PlayButton_PlayIcon;
		Ref<Texture2D> m_PlayButton_StopIcon;
		Ref<Texture2D> m_PlayButton_CurrIcon;
		char* input_buff = new char[50];
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

		void BeginDialogue_OpenProject() {
			std::string filepath = FileDialogs::OpenFile(ProjectManager::GetProjectsDirectory(),"Surf Project (*.surf)\0*.surf\0");
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

			s_EditorCamera.reset(new OrthographicCamera());
			
			s_EditorCamera->SetZoom(4.0f);

				
			//Create the FrameBuffer
			FramebufferSpecification fbSpec;
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA8 , FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
			fbSpec.Width = 1920;
			fbSpec.Height = 1080;
			Renderer2D::SetRenderTarget(Framebuffer::Create(fbSpec));
		}

		void OnUpdate(Timestep timestep) override {
			//RenderScene
			if (ProjectManager::IsActiveScene()) {
				auto& scene = ProjectManager::GetActiveScene();
				if (s_play) {
					scene->OnUpdateRuntime(timestep);
				}
				else {
					scene->OnUpdateEditor(timestep, s_EditorCamera, s_draw_grid);
					if (viewport_is_selected) {
						s_EditorCamera->OnUpdate(timestep);
					}
				}
			}
		}

		void OnEvent(Event& event) override {
			if (viewport_is_selected && !s_play) {
				s_EditorCamera->OnEvent(event);
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
