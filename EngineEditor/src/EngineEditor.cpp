#include "MechEngine.h"
#include "MechEngine/Core/EntryPoint.h"

#include "Panels/Panel_Hierarchy.h"
#include "Panels/Panel_Inspector.h"
#include "Panels/Panel_Viewport.h"

#include "imgui/imgui.h"
#include "imgui/imconfig.h"

namespace MechEngine {

	static Ref<Scene> m_ActiveScene;
	static Ref<OrthographicCamera> sceneCamera;

	class EditorLayer : public Layer {
	public:
		EditorLayer() :
			Layer("Editor")

		{}

		void OnAttach() override {
			m_ActiveScene = std::make_shared<Scene>();
			m_panel_hierarchy = std::make_shared<Panel_Hierarchy>(m_ActiveScene);
			m_panel_inspector = std::make_shared<Panel_Inspector>(m_panel_hierarchy);
			m_panel_viewport = std::make_shared<Panel_Viewport>();
		}

		void OnImGuiRender() override {
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
					if (ImGui::MenuItem("New", "CTRL+N")) {
					}
					if (ImGui::MenuItem("Open", "CTRL+O")) { OpenScene("C:\\Users\\timbe\\source\\repos\\MechEngine\\Default.scene"); }
					if (ImGui::MenuItem("Save", "CTRL+S")) { SaveScene("C:\\Users\\timbe\\source\\repos\\MechEngine\\Default.scene"); }
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("GameObject")) {
					if (ImGui::MenuItem("Add GameObject")) {
						Object square = m_ActiveScene->CreateObject();
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
				ImGui::EndMainMenuBar();

				//Declare Central dockspace
				m_DockspaceId = ImGui::GetID("HUB_DockSpace");
				ImGui::DockSpace(m_DockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode/*|ImGuiDockNodeFlags_NoResize*/);
			}
			ImGui::End();






			m_panel_viewport->OnImGuiRender();
			m_panel_hierarchy->OnImGuiRender();
			m_panel_inspector->OnImGuiRender();



			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}

	private:
		Ref<Panel_Hierarchy> m_panel_hierarchy;
		Ref<Panel_Inspector> m_panel_inspector;
		Ref<Panel_Viewport> m_panel_viewport;
		bool  m_GuiIsActive = true;
		ImGuiID m_DockspaceId = 0;
	private:
		void OpenScene(const std::string& filepath) {
			Ref<Scene> openedScene = std::make_shared<Scene>();
			SceneSerializer serializer(openedScene);
			serializer.Deserialze(filepath);
			m_ActiveScene = openedScene;
			m_panel_hierarchy->SetActiveScene(m_ActiveScene);

		}

		void SaveScene(const std::string& filepath) {
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialze(filepath);
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
			if (true) {
				sceneCamera->OnUpdate(timestep);

			}

			//RenderScene
			Renderer2D::BeginScene(sceneCamera);
			Renderer2D::DrawBackgroundGrid(1.0f);
			m_ActiveScene->OnUpdate(timestep);
			Renderer2D::EndScene();
		}

		void OnEvent(Event& event) override {
			if (true) {
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
