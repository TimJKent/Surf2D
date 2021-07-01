
#include "MechEngine.h"
#include "MechEngine/Core/EntryPoint.h"

#include "imgui/imgui.h"
#include "imgui/imconfig.h"
#include "ScreenList.h"
#include "ScreenComponent.h"
#include "Group.h"



static MechEngine::ObjectContainer s_ObjectList;
static float s_GridSize = 1.0f;

static glm::vec2 s_ViewPortPosition = { 0,0 };
static glm::vec2 s_ViewPortSize = { 0,0 };

static bool s_ViewModeIs3d = true;
static bool s_ViewportIsSelected = false;

static std::vector<Group> s_Groups;

static MechEngine::Ref<MechEngine::Camera> s_CameraP;
static MechEngine::Ref<MechEngine::Camera> s_CameraO;

class EditorLayer : public MechEngine::Layer {
public:
	EditorLayer() : 
		Layer("Editor"),
		m_ApplicationWindowSize({ 0,0 }), 
		m_ApplicationWindowPosition({ 0,0 })
	{
	}

	void OnAttach() override {
		MechEngine::Window& window = MechEngine::Application::Get().GetWindow();
		m_ApplicationWindowSize = glm::vec2(window.GetWidth(), window.GetHeight());
		m_ApplicationWindowPosition = glm::vec2(window.GetPosX(), window.GetPosY());
	}

	void OnImGuiRender() override {
		//Initilize Dockspace
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.7f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
		if (ImGui::Begin("Screen Mesh", &m_GuiIsActive, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
		{
			ImGui::SetWindowSize({ m_ApplicationWindowSize.x, m_ApplicationWindowSize.y -20});
			ImGui::SetWindowPos({ m_ApplicationWindowPosition.x, m_ApplicationWindowPosition.y +20});
		}
		if (m_GuiIsActive)
		{
			ImGui::BeginMainMenuBar();
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New"))  { NewScene(); }
				if (ImGui::MenuItem("Open")) { OpenScene(); }
				if (ImGui::MenuItem("Save")) { SaveScene(); }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Objects")) {
				if (ImGui::MenuItem("New Object")) { AddObject(); }
				ImGui::Separator();
				if (ImGui::MenuItem("New Screen")) { AddScreen(); }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Componenets")) {
				if (ImGui::MenuItem("Add Transform")) {if(!s_ObjectList.IsEmpty()) s_ObjectList.GetSelected()->AddComponent<MechEngine::TransformComponent>();}
				if (ImGui::MenuItem("Add Mesh Renderer 2d")) { if (!s_ObjectList.IsEmpty()) s_ObjectList.GetSelected()->AddComponent<MechEngine::MeshRenderer2dComponent>(); }
				if (ImGui::MenuItem("Add Mesh Renderer 3d")) { if (!s_ObjectList.IsEmpty()) s_ObjectList.GetSelected()->AddComponent<MechEngine::MeshRenderer3dComponent>(); }
				if (ImGui::MenuItem("Add Screen Controller")) { if (!s_ObjectList.IsEmpty())s_ObjectList.GetSelected()->AddComponent<ScreenComponent>(); }

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
			
			//Declare Central dockspace
			m_DockspaceId = ImGui::GetID("HUB_DockSpace");
			ImGui::DockSpace(m_DockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode/*|ImGuiDockNodeFlags_NoResize*/);
		}
		ImGui::End();

		ImGui::ShowDemoWindow();

//ViewPort
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f,0.0f });
		if (ImGui::Begin("ViewPort")) {
			s_ViewportIsSelected = ImGui::IsWindowFocused();
			s_ViewPortPosition = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
			s_ViewPortSize = { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y};
			ImVec2 viewPortSize = ImGui::GetContentRegionAvail();
			if (viewPortSize.x != viewPortSize.x || viewPortSize.y != m_ViewPortSize.y) {
				m_ViewPortSize = { viewPortSize.x, viewPortSize.y };
				MechEngine::Renderer2D::ResizeRenderTarget(m_ViewPortSize.x, m_ViewPortSize.y);
			}

			//Ask our fbo to put its output into a texture
			uint32_t textureID = MechEngine::Renderer2D::GetOutputAsTextureId();
			//clamp its vertical height to the renderer asepect ratio - important to prevent distortion
			ImVec2 size = ImVec2(viewPortSize.x, viewPortSize.y);
			//Render the clamped image onto the window
			ImGui::Image((void*)textureID, size);
		}
		ImGui::End();
		ImGui::PopStyleVar();

//Screen Properties
		if (ImGui::Begin("Properties") && !s_ObjectList.IsEmpty())
		{
			DrawObjectUI(s_ObjectList.GetSelected());
			
		}
		ImGui::End();

//Groups
		if (ImGui::Begin("Groups"))
		{
			if (ImGui::Button("Add Group")) {
				s_Groups.push_back(Group(s_Groups.size()));
			}

			for (int i = 0; i < s_Groups.size(); i++){
				std::string name = "Group " + std::to_string(s_Groups[i].GetId());
				MechEngine::Color color = s_Groups[i].GetColor();
				if (ImGui::ColorEdit3(name.c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
					s_Groups[i].SetColor(color);
				}
				
				ImGui::SameLine();
				if (ImGui::TreeNode(name.c_str())) {
					
					for (int j = 0; j < s_Groups[i].Size(); j++) {
						ImGui::Text(s_Groups[i].GetScreen(j).GetName().c_str());
					}
					ImGui::TreePop();
				}
			}
		}
		ImGui::End();
		
		

//Hierarchy
		if (ImGui::Begin("Hierarchy"))
		{
			//List Objects in Hierarchy
			for (int n = 0; n < s_ObjectList.Size(); n++)
			{
				std::string id = "Delete" + std::to_string(n);
				ImGui::PushID(id.c_str());
				if (ImGui::Button("X")) {
					s_ObjectList.Delete(n);
				}	
				ImGui::PopID();
				ImGui::SameLine();
				std::string hiddenIcon = !s_ObjectList.Get(n)->m_Enabled ? " H" : "";
				std::string selectableName = s_ObjectList.Get(n)->GetName() + hiddenIcon;
				if (ImGui::Selectable(selectableName.c_str(), s_ObjectList.GetSelectedNumber() == n)) {
					s_ObjectList.SetSelectedObject(n);
				}
				
			}
		}
		ImGui::End();
		
//Toolbar
		if (ImGui::Begin("Toolbar")) {
			ImGui::Text("Grid Size");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.f);
			ImGui::PushID("gridSize");
			ImGui::InputFloat("", &s_GridSize, 1.0);
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::Text("3D");
			ImGui::SameLine();
			ImGui::PushID("3dMode");
			if (ImGui::Checkbox("", &s_ViewModeIs3d)) {
				SwitchViewModes();
			}
			ImGui::PopID();
		}
		ImGui::End();

		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}

	void OnEvent(MechEngine::Event& event) override {
		if (event.GetEventType() == MechEngine::EventType::WindowResize) {
			MechEngine::WindowResizeEvent& e = (MechEngine::WindowResizeEvent&)event;
			m_ApplicationWindowSize.x = e.GetWidth();
			m_ApplicationWindowSize.y = e.GetHeight();
			float aspectRatio = m_ApplicationWindowSize.x / m_ApplicationWindowSize.y;
			s_CameraP->SetProjection(glm::perspective(glm::radians(66.f), aspectRatio, 0.1f, 100.f));
		}
		if (event.GetEventType() == MechEngine::EventType::WindowMoved) {
			MechEngine::WindowMoveEvent& e = (MechEngine::WindowMoveEvent&)event;
			m_ApplicationWindowPosition.x = e.GetPosX();
			m_ApplicationWindowPosition.y = e.GetPosY();
		}
		if (event.GetEventType() == MechEngine::EventType::KeyPressed) {
			MechEngine::KeyPressedEvent& e = (MechEngine::KeyPressedEvent&)event;
			switch (e.GetKeyCode()) {
			case ME_KEY_TAB:	if (s_ViewportIsSelected) { s_ViewModeIs3d = !s_ViewModeIs3d; SwitchViewModes(); } break;
				default: break;
			}
		}
	}

	void SwitchViewModes() {
		MechEngine::Renderer2D::SetRenderMode(!MechEngine::Renderer2D::GetRenderMode());
	}
	
	void NewScene() {
		MechEngine::Serialization::CreateFileW("C:\\Users\\timbe\\Desktop\\testFolder", "test");
	}

	void SaveScene() {
		s_ObjectList.Save();
	}

	void OpenScene() {
		s_ObjectList.DeleteAll();
		int numberOfScreensToLoad;
		MechEngine::Serialization::OpenFileForReadAndWrite("C:\\Users\\timbe\\Desktop\\testFolder", "test");
		MechEngine::Serialization::SERIAL_READ(&numberOfScreensToLoad);
		for (int i = 0; i < numberOfScreensToLoad; i++) {
			MechEngine::Ref<MechEngine::Object> o = std::make_shared<MechEngine::Object>();
			o->Load();
			int numComponents = 0;
			MechEngine::Serialization::SERIAL_READ(&numComponents);
			for (int j = 0; j < numComponents; j++) {
				std::string componentId = "";
				MechEngine::Serialization::SERIAL_READ(&componentId);
				if (componentId == MechEngine::TransformComponent::StaticId()) { o->AddComponent<MechEngine::TransformComponent>(); }
				if (componentId.compare(MechEngine::MeshRenderer3dComponent::StaticId()) == 0) { o->AddComponent<MechEngine::MeshRenderer3dComponent>(); }
				if (componentId.compare(MechEngine::MeshRenderer2dComponent::StaticId()) == 0) { o->AddComponent<MechEngine::MeshRenderer2dComponent>(); }
				if (componentId.compare(ScreenComponent::StaticId()) == 0) { o->AddComponent<ScreenComponent>(); }
			}
			o->LoadComponents();
			s_ObjectList.Add(o);
			
		}
		MechEngine::Serialization::CloseFile();
		if (s_ObjectList.Size() > 0) {
			s_ObjectList.SetSelectedObject(0);
		}
	}

	void AddObject() { s_ObjectList.AddDefaultObject(); }
	
	void AddScreen() { 
		MechEngine::Ref<MechEngine::Object> o = std::make_shared<MechEngine::Object>();
		o->AddComponent<ScreenComponent>();
		s_ObjectList.Add(o);
	}

	void DrawObjectUI(MechEngine::Ref<MechEngine::Object> object) {
		char* name = new char[object->GetName().size() + 10];
		strcpy(name, object->GetName().c_str());
		ImGui::PushID("Visible");
		ImGui::Checkbox("", &object->m_Enabled);
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::PushID("Name");
		if (ImGui::InputText("", name, 20, ImGuiInputTextFlags_CharsNoBlank|ImGuiInputTextFlags_EnterReturnsTrue)) {
			if(!s_ObjectList.ContainsName(name))
				object->SetName(name);
		}
		ImGui::PopID();
		ImGui::Separator();

		for (int i = 0; i < object->GetNumberOfComponents(); i++) {
			bool alwaysFalse = false;
			std::string enabledCheckBoxId = "Enabled" + std::to_string(i);
			ImGui::PushID(enabledCheckBoxId.c_str());
			if (object->m_Enabled) {
				ImGui::Checkbox("", &object->GetComponent<MechEngine::Component>(i)->IsEnabled);
			}
			else {
				ImGui::Checkbox("", &alwaysFalse);
				alwaysFalse = false;
			}
			ImGui::PopID();
			ImGui::SameLine();
			std::string deleteButtonId = "Delete" + std::to_string(i);
			ImGui::PushID(deleteButtonId.c_str());
			if (ImGui::BeginPopup("CompOptions")) {
				if (ImGui::Selectable("Move Up")) { object->MoveComponentUp(i); };
				if (ImGui::Selectable("Move Down")) { object->MoveComponentDown(i); };
				ImGui::Separator();
				if (ImGui::Selectable("Delete")) { object->RemoveComponent(i); };
				ImGui::EndPopup();
			}
			if (ImGui::Button("V")) { ImGui::OpenPopup("CompOptions"); }
			ImGui::PopID();
			ImGui::SameLine();
			std::string componentId = "Comp" + std::to_string(i);
			ImGui::PushID(componentId.c_str());
			object->GetComponent<MechEngine::Component>(i)->DrawUI();
			ImGui::PopID();
		}
	}


private:
	glm::vec2 m_ApplicationWindowSize;
	glm::vec2 m_ApplicationWindowPosition;
	glm::vec2 m_ViewPortSize = { 0,0 };
	bool  m_GuiIsActive = true;
    ImGuiID m_DockspaceId = 0;
};

class RenderLoopLayer : public MechEngine::Layer {

public:
	RenderLoopLayer() : Layer("Example"), sceneCamera(s_CameraP) {
		
	}

	void OnAttach() override {
		//Enable Textures, Important for FBO to work correctly - probably should be moved
		MechEngine::RenderCommand::EnableTextures();
		MechEngine::RenderCommand::EnableBlending();
		MechEngine::RenderCommand::EnableDepthTesting();
		MechEngine::RenderCommand::EnableMSAA();
		MechEngine::Renderer2D::Init();

		s_CameraP.reset(new MechEngine::PerspectiveCamera());
		s_CameraO.reset(new MechEngine::OrthographicCamera());
		s_CameraP->m_Transform.SetPosition({1.f,-0.5f,1.f});
		s_CameraP->m_Transform.Rotate({ -15.f,-45.f,0.0f });

		//Create Window Material
		MechEngine::Ref<MechEngine::Material> mat_Texture = MechEngine::Material::Create();
		mat_Texture->SetShader(MechEngine::Shader::Create("assets/shaders/texture.glsl"));
		mat_Texture->AddTexture(MechEngine::Texture2D::Create("assets/textures/windows.png"));
		MechEngine::Renderer2D::PushMaterial(mat_Texture);

		//Create Border Material
		MechEngine::Ref<MechEngine::Material> mat_Border = MechEngine::Material::Create();
		mat_Border->SetShader(MechEngine::Shader::Create("assets/shaders/color.glsl"));
		mat_Border->Bind();
		mat_Border->GetShader()->SetFloat4("u_Color", {1.f,1.f,1.f,1.f});
		MechEngine::Renderer2D::PushMaterial(mat_Border);

		//Create Mesh Material
		MechEngine::Ref<MechEngine::Material> mat_WireFrame = MechEngine::Material::Create();
		mat_WireFrame->SetShader(MechEngine::Shader::Create("assets/shaders/shader_mesh.glsl"));
		MechEngine::Renderer2D::PushMaterial(mat_WireFrame);

		//Create the FrameBuffer
		MechEngine::FramebufferSpecification fbSpec;
		fbSpec.Attachments = { MechEngine::FramebufferTextureFormat::RGBA8 , MechEngine::FramebufferTextureFormat::RED_INTEGER, MechEngine::FramebufferTextureFormat::Depth};
		fbSpec.Width = 1920;
		fbSpec.Height = 1080;
		fbSpec.Samples = 1;
		MechEngine::Renderer2D::SetRenderTarget(MechEngine::Framebuffer::Create(fbSpec));
	}

	void OnUpdate(MechEngine::Timestep timestep) override {

		//SetSceneCamer
		 sceneCamera = s_ViewModeIs3d ? s_CameraP : s_CameraO;
		
		//UpdateObjects
		if (s_ViewportIsSelected) {
			sceneCamera->OnUpdate(timestep);
			CheckInput();
		}
		
		//RenderScene
		MechEngine::Renderer2D::BeginScene(sceneCamera);
		    s_ObjectList.UpdateList();
			MechEngine::Renderer2D::DrawBackgroundGrid(s_GridSize, s_ViewModeIs3d);
		MechEngine::Renderer2D::EndScene();
	}

	void OnEvent(MechEngine::Event& event) override {
		if (s_ViewportIsSelected) {
			if(!s_ViewModeIs3d)
				s_CameraO->OnEvent(event);
		}
	}

private:
	bool mouseCheck = true;
	MechEngine::Ref<MechEngine::Camera> sceneCamera;

private:
	void CheckInput() {

		//if (MechEngine::Input::IsMouseButtonPressed(ME_MOUSE_BUTTON_1)) {
		//	glm::vec2 m = GetMouseWorldCooridnates();
		//	if (MechEngine::Input::IsKeyPressed(ME_KEY_LEFT_SHIFT)) {
		//		m.x = std::floor(m.x / s_GridSize) * s_GridSize;
		//		m.y = std::floor(m.y / s_GridSize) * s_GridSize;
		//	}
		//	s_ScreenList.MoveVerts(m.x,m.y);
		//}

		//Used for vertex selection, currently not working
		//if (MechEngine::Input::IsMouseButtonPressed(ME_MOUSE_BUTTON_2)) {
		//	if (!s_ViewModeIs3d && mouseCheck) {
		//		mouseCheck = false;
		//		s_ScreenList.SetSelectedPoint(GetMouseWorldCooridnates(), MechEngine::Input::IsKeyPressed(ME_KEY_LEFT_CONTROL));
		//	}
		//}
		//else {
		//	mouseCheck = true;
		//}
	}

	void PrintMosaicLayout() {
		//int mosaicRows = ScreenMesh::GetRows(s_ScreenList);
		//int mosaicColumns = ScreenMesh::GetColumns(s_ScreenList);
		//std::string displayID = "";
		//ScreenMesh::Sort(s_ScreenList);
		//
		//for (int i = 0; i < s_ScreenList.size(); i++) {
		//	displayID += ",";
		//	displayID += s_ScreenList[i]->GetDisplayId();
		//}
		//ME_INFO("Rows: {0} Columns: {1} {2}", mosaicRows, mosaicColumns, displayID);
	}

	glm::vec2 GetMouseWorldCooridnates() {
		//glm::vec2 mousePosition = {0,0};
		//glm::vec2 applicationPosition = {0,0};
		//glm::vec2 absoluteViewportPosition = { 0,0 };
		//
		//applicationPosition.x = MechEngine::Application::Get().GetWindow().GetPosX();
		//applicationPosition.y = MechEngine::Application::Get().GetWindow().GetPosY();
		//
		//absoluteViewportPosition.x = s_ViewPortPosition.x - applicationPosition.x;
		//absoluteViewportPosition.y = s_ViewPortPosition.y - applicationPosition.y;
		//
		//mousePosition = { MechEngine::Input::GetMouseX(),MechEngine::Input::GetMouseY() };
		//
		//glm::vec2 mouseInsideViewPort = {(mousePosition.x- absoluteViewportPosition.x)/s_ViewPortSize.x,(mousePosition.y - absoluteViewportPosition.y) / s_ViewPortSize.y };
		//
		//glm::vec4 cameraBounds = s_CameraController->GetCamera().GetBounds();
		//
		//glm::vec2 worldSpaceCoordinate = {
		//	cameraBounds.r + mouseInsideViewPort.x * (cameraBounds.g - cameraBounds.r) + s_CameraController->GetCamera().GetTransform().GetPosition().x,
		//	cameraBounds.b + mouseInsideViewPort.y * (cameraBounds.a- cameraBounds.b) + s_CameraController->GetCamera().GetTransform().GetPosition().y,
		//};
		//
		//return worldSpaceCoordinate;
		return { 0.0f,0.0f };
	}
};

class Sandbox : public MechEngine::Application
{
public:
	Sandbox() {
		PushLayer(new EditorLayer());
		PushLayer(new RenderLoopLayer());
	}

	~Sandbox() {
	
	}
	
private:
};

MechEngine::Application* MechEngine::CreateApplication() {
	return new Sandbox();
}

