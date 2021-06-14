#include "MechEngine.h"
#include "MechEngine/EntryPoint.h"

#include "imgui/imgui.h"
#include "imgui/imconfig.h"
#include "ScreenList.h"
#include "Group.h"

#include "UIUtil.h"

static ScreenList s_ScreenList;

static float s_GridSize = 1.0f;

static MechEngine::Ref<MechEngine::Mesh> s_Mesh;

static float m_LastZoom = 1.0f;
static glm::vec3 s_3dCameraPosition = { 0,0,0 };
static glm::vec3 s_3dCameraRotation = { 0, 0, 0 };
				 
static glm::vec3 s_2dCameraPosition = { 0,0,0 };
static glm::vec3 s_2dCameraRotation = { 0, 0, 0 };

static glm::vec2 s_ViewPortPosition = { 0,0 };
static glm::vec2 s_ViewPortSize = { 0,0 };

static bool s_ViewModeIs3d = true;
static bool s_ViewportIsSelected = false;


static std::vector<Group> s_Groups;

static MechEngine::Ref<MechEngine::Camera> s_Camera;

class EditorLayer : public MechEngine::Layer {
public:
	EditorLayer() : Layer("Editor") {
		
	}

	void OnAttach() override {
		

		m_ApplicationWindowSize = glm::vec2(
			MechEngine::Application::Get().GetWindow().GetWidth(),
			MechEngine::Application::Get().GetWindow().GetHeight()
			);
		m_ApplicationWindowPosition = glm::vec2(
			MechEngine::Application::Get().GetWindow().GetPosX(),
			MechEngine::Application::Get().GetWindow().GetPosY()
			);
	}

	void OnImGuiRender() override {
		//Prevents Application from crashin on minimize, surely there is a better engine side solution
		if (m_ApplicationWindowSize.x == 0 || m_ApplicationWindowSize.y == 0) {
			return;
		}
		
//Initilize Dockspace
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.7f));
		if (ImGui::Begin("Screen Mesh", &m_GuiIsActive, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
		{
			ImGui::SetWindowSize({ m_ApplicationWindowSize.x, m_ApplicationWindowSize.y });
			ImGui::SetWindowPos({ m_ApplicationWindowPosition.x, m_ApplicationWindowPosition.y });
		}
		if (m_GuiIsActive)
		{
			int height;
			ImGui::BeginMainMenuBar();
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New"))  { NewScene(); }
				if (ImGui::MenuItem("Open")) { OpenScene(); }
				if (ImGui::MenuItem("Save")) { SaveScene(); }
				if (ImGui::MenuItem("Exit")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
			
			//Declare Central dockspace
			m_DockspaceId = ImGui::GetID("HUB_DockSpace");
			ImGui::DockSpace(m_DockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode/*|ImGuiDockNodeFlags_NoResize*/);
		}
		ImGui::End();


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
		if (ImGui::Begin("Screen Properties") && !s_ScreenList.IsEmpty())
		{
			bool visible = !s_ScreenList.GetSelected()->IsHidden();
			bool primary =  s_ScreenList.GetSelected()->IsPrimary();
			glm::vec2 position = s_ScreenList.GetSelected()->transform2d.GetPosition();
			glm::vec3 tdposition = s_ScreenList.GetSelected()->transform3d.GetPosition();
			glm::vec3 tdrotation = s_ScreenList.GetSelected()->transform3d.GetRotation();
			glm::vec2 resolution = s_ScreenList.GetSelected()->GetResolution();
			char* displayId = new char[s_ScreenList.GetSelected()->GetDisplayId().length() + 10];
			char* name = new char[s_ScreenList.GetSelected()->GetName().length() + 10];
			strcpy(name, s_ScreenList.GetSelected()->GetName().c_str());
			strcpy(displayId, s_ScreenList.GetSelected()->GetDisplayId().c_str());

			ImGui::PushID("Visible");
			ImGui::Checkbox("", &visible);
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::PushID("Name");
			ImGui::InputText("", name, 20);
			ImGui::PopID();
			ImGui::Text("DID");
			ImGui::SameLine();
			ImGui::PushID("ID");
			ImGui::InputText("", displayId, 20);
			ImGui::PopID();
			ImGui::SetNextItemWidth(100);
			if (ImGui::Checkbox("Primary", &primary)) {
				s_ScreenList.SetPrimary(s_ScreenList.GetSelectedNumber());
			}
			ImGui::NewLine();


			if (s_ViewModeIs3d) {
				ImGui::Text("3D Transform");
				ImGui::Separator();
				Draw3DTransformUI(&tdposition, "Pos3D");
				Draw3DTransformUI(&tdrotation, "Rot3D");
				ImGui::NewLine();
			}
			else {
				ImGui::Text("2D Transform");
				ImGui::Separator();
				Draw2DTransformUI(&position, "Pos");
				Draw2DTransformUI(&resolution, "Res");
				ImGui::NewLine();
			}


			//Update Properties based off of UI changes
			s_ScreenList.GetSelected()->SetHidden(!visible);
			s_ScreenList.GetSelected()->transform2d.SetPosition(position);
			s_ScreenList.GetSelected()->transform3d.SetPosition(tdposition);
			s_ScreenList.GetSelected()->transform3d.SetRotation(tdrotation);
			s_ScreenList.GetSelected()->SetResolution(resolution);
			s_ScreenList.GetSelected()->SetDisplayId(displayId);
			s_ScreenList.GetSelected()->SetName(name);
			if (ImGui::Button("Delete")) {
				s_ScreenList.DeleteSelected();
			}
			ImGui::Separator();
			std::string currentGroupName = "No Group";
			int groupId = s_ScreenList.GetSelected()->GetGroupId();
			if (groupId >= 0) { currentGroupName = "Group " + std::to_string(groupId); }
			if (ImGui::BeginCombo("Group", currentGroupName.c_str())) {
				if (ImGui::Selectable("No Group")) {
					int currGroup = s_ScreenList.GetSelected()->GetGroupId();
					if (currGroup > 0 && currGroup < s_Groups.size()) {
						s_Groups[currGroup].RemoveScreen(s_ScreenList.GetSelected()->GetName());
					}
					s_ScreenList.GetSelected()->SetGroup(-1);
				}
				for (int i = 0; i < s_Groups.size(); i++) {
					std::string name = "Group " + std::to_string(s_Groups[i].GetId());
					if (ImGui::Selectable(name.c_str())) {
						s_Groups[i].AddScreen(s_ScreenList.GetSelected());
						s_ScreenList.GetSelected()->SetGroup(s_Groups[i].GetId());
					}
				}
				ImGui::EndCombo();
			}
			
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
			//Spawn Window Button
			if (ImGui::Button("Spawn Window")) {
				s_ScreenList.AddScreen();
			}

			//List Objects in Hierarchy
			for (int n = 0; n < s_ScreenList.Size(); n++)
			{
				std::string hiddenIcon = s_ScreenList.Get(n)->IsHidden() ? " H" : "";
				std::string primaryIcon = s_ScreenList.Get(n)->IsPrimary() ? "*" : "";
				std::string selectableName = primaryIcon + s_ScreenList.Get(n)->GetName() + hiddenIcon;
				if (ImGui::Selectable(selectableName.c_str(), s_ScreenList.GetSelectedNumber() == n)) {
					s_ScreenList.SetSelectedScreen(n);
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

		ImGui::PopStyleColor();
	}

	void OnEvent(MechEngine::Event& event) override {
		if (event.GetEventType() == MechEngine::EventType::WindowResize) {
			MechEngine::WindowResizeEvent& e = (MechEngine::WindowResizeEvent&)event;
			m_ApplicationWindowSize.x = e.GetWidth();
			m_ApplicationWindowSize.y = e.GetHeight();
			float aspectRatio = m_ApplicationWindowSize.x / m_ApplicationWindowSize.y;
			s_Camera->SetProjection(glm::perspective(glm::radians(66.f), aspectRatio, 0.1f, 100.f));
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
		//if (s_ViewModeIs3d) {
		//	s_2dCameraPosition = s_CameraController->GetCamera().GetTransform().GetPosition();
		//	s_2dCameraRotation = s_CameraController->GetCamera().GetTransform().GetRotation();
		//	m_LastZoom = s_CameraController->GetZoom();
		//	s_CameraController.reset(new MechEngine::PerspectiveCameraController(1920.0f / 1080.0f));
		//	s_CameraController->transform.SetPosition(s_3dCameraPosition);
		//	s_CameraController->transform.SetRotation(s_3dCameraRotation);
		//}
		//else {
		//	s_3dCameraPosition = s_CameraController->GetCamera().GetTransform().GetPosition();
		//	s_3dCameraRotation = s_CameraController->GetCamera().GetTransform().GetRotation();
		//	s_CameraController.reset(new MechEngine::OrthographicCameraController(1920.0f / 1080.0f));
		//	s_CameraController->SetZoom(m_LastZoom);
		//	s_CameraController->transform.SetPosition(s_2dCameraPosition);
		//	s_CameraController->transform.SetRotation(s_2dCameraRotation);
		//}
	}
	
	void NewScene() {
		MechEngine::Serialization::CreateFileW("C:\\Users\\timbe\\Desktop\\testFolder", "test");
	}

	void SaveScene() {
		MechEngine::Serialization::OpenFileForReadAndWrite("C:\\Users\\timbe\\Desktop\\testFolder", "test");
		MechEngine::Serialization::SERIAL_WRITE(s_ScreenList.Size());
		for (int i = 0; i < s_ScreenList.Size(); i++) {
			s_ScreenList.Get(i)->SERIAL_WRITE();
		}
		MechEngine::Serialization::CloseFile();
	}

	void OpenScene() {
		s_ScreenList.DeleteAll();
		int numberOfScreensToLoad;
		MechEngine::Serialization::OpenFileForReadAndWrite("C:\\Users\\timbe\\Desktop\\testFolder", "test");
		MechEngine::Serialization::SERIAL_READ(&numberOfScreensToLoad);
		for (int i = 0; i < numberOfScreensToLoad; i++) {
			s_ScreenList.AddScreen();
			s_ScreenList.Get(i)->SERIAL_READ();
		}
		MechEngine::Serialization::CloseFile();
	}

private:
	glm::vec2 m_ApplicationWindowSize;
	glm::vec2 m_ApplicationWindowPosition;
	glm::vec2 m_ViewPortSize = { 0,0 };
	bool  m_GuiIsActive = true;
    ImGuiID m_DockspaceId = 0;
};

class ExampleLayer : public MechEngine::Layer {

public:
	ExampleLayer() : Layer("Example") {
		
	}

	void OnAttach() override {
		//Enable Textures, Important for FBO to work correctly - probably should be moved
		MechEngine::RenderCommand::EnableTextures();
		MechEngine::RenderCommand::EnableBlending();
		MechEngine::RenderCommand::EnableDepthTesting();
		MechEngine::RenderCommand::EnableMSAA();
		MechEngine::Renderer2D::Init();

		s_Camera.reset(new MechEngine::PerspectiveCamera());
		s_Camera->m_Transform.SetPosition({1.f,-0.5f,1.f});
		s_Camera->m_Transform.Rotate({ -15.f,-45.f,0.0f });

		//Create Window Material
		MechEngine::Ref<MechEngine::Material> mat_Texture = MechEngine::Material::Create();
		mat_Texture->SetShader(MechEngine::Shader::Create("assets/shaders/texture.glsl"));
		mat_Texture->AddTexture(MechEngine::Texture2D::Create("assets/textures/windows.png"));
		MechEngine::Renderer2D::PushMaterial(mat_Texture);

		//Create Border Material
		MechEngine::Ref<MechEngine::Material> mat_Texture2 = MechEngine::Material::Create();
		mat_Texture2->SetShader(MechEngine::Shader::Create("assets/shaders/color.glsl"));
		mat_Texture2->Bind();
		mat_Texture2->GetShader()->SetFloat4("u_Color", {1.f,1.f,1.f,1.f});
		MechEngine::Renderer2D::PushMaterial(mat_Texture2);

		//Create Mesh Material
		MechEngine::Ref<MechEngine::Material> mat_Mesh = MechEngine::Material::Create();
		mat_Mesh->SetShader(MechEngine::Shader::Create("assets/shaders/shader_mesh.glsl"));
		mat_Mesh->Bind();
		MechEngine::Renderer2D::PushMaterial(mat_Mesh);

		//Create Window Highlight Material
		//MechEngine::Ref<MechEngine::Material> mat_FlatColor = MechEngine::Material::Create();
		//mat_FlatColor->SetShader(MechEngine::Shader::Create("assets/shaders/color.glsl"));
		//mat_FlatColor->GetShader()->SetFloat4("u_Color", {1.f,0.f,0.f,1.f});
		//MechEngine::Renderer2D::PushMaterial(mat_FlatColor);



		//Create the FrameBuffer
		MechEngine::FramebufferSpecification fbSpec;
		fbSpec.Attachments = { MechEngine::FramebufferTextureFormat::RGBA8 , MechEngine::FramebufferTextureFormat::RED_INTEGER, MechEngine::FramebufferTextureFormat::Depth};
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		fbSpec.Samples = 1;
		MechEngine::Renderer2D::SetRenderTarget(MechEngine::Framebuffer::Create(fbSpec));
	}

	void OnUpdate(MechEngine::Timestep timestep) override {
		if (s_ViewportIsSelected) {
			s_Camera->OnUpdate(timestep);
			CheckInput();
		}
		
		//Render Scene
		MechEngine::Renderer2D::BeginScene(s_Camera);
		s_ScreenList.DrawList(s_ViewModeIs3d);
		MechEngine::Renderer2D::DrawBackgroundGrid(s_GridSize, s_ViewModeIs3d);
		MechEngine::Renderer2D::EndScene();
	}

	void OnEvent(MechEngine::Event& event) override {
		if (s_ViewportIsSelected) {
			//s_Camera->OnEvent(event);
		}
	}

private:
	bool mouseCheck = true;

private:
	void CheckInput() {

		if (MechEngine::Input::IsMouseButtonPressed(ME_MOUSE_BUTTON_1)) {
			glm::vec2 m = GetMouseWorldCooridnates();
			if (MechEngine::Input::IsKeyPressed(ME_KEY_LEFT_SHIFT)) {
				m.x = std::floor(m.x / s_GridSize) * s_GridSize;
				m.y = std::floor(m.y / s_GridSize) * s_GridSize;
			}
			s_ScreenList.MoveVerts(m.x,m.y);
		}

		if (MechEngine::Input::IsMouseButtonPressed(ME_MOUSE_BUTTON_2)) {
			if (!s_ViewModeIs3d && mouseCheck) {
				mouseCheck = false;
				s_ScreenList.SetSelectedPoint(GetMouseWorldCooridnates(), MechEngine::Input::IsKeyPressed(ME_KEY_LEFT_CONTROL));
			}
		}
		else {
			mouseCheck = true;
		}

		if (MechEngine::Input::IsKeyPressed(ME_KEY_F)) {
			//s_CameraController->transform.SetPosition(s_ScreenList.FocusPositionOnSelected());
			//s_CameraController->transform.SetRotation(s_ScreenList.FocusRotationOnSelected());
		}
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
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {
	
	}
	
private:
};

MechEngine::Application* MechEngine::CreateApplication() {
	return new Sandbox();
}

