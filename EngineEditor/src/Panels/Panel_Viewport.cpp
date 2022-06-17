#include "sepch.h"

#include "Panel_Viewport.h"
#include "../Util/ProjectManager.h"
#include "../Util/MenuManager.h"
#include "SurfEngine/Renderer/Renderer2D.h"
#include "SurfEngine/Scenes/ObjectSerializer.h"




namespace SurfEngine {
		
	
	const char* items[] = { "1920x1080", "1600x900", "1600x1000", "1080x1920"};
	static const char* current_item = items[0];


	Panel_Viewport::Panel_Viewport() {
		m_PlayButton_PlayIcon = Texture2D::Create("res\\textures\\icon_play.png");
		m_PlayButton_StopIcon = Texture2D::Create("res\\textures\\icon_stop.png");
		m_PlayButton_CurrIcon = m_PlayButton_PlayIcon;
	}

	void Panel_Viewport::UpdateViewPortSize() {
		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		m_ViewPortSize = windowSize;
		float aspectRatio = Renderer2D::GetRenderTargetSize().x / Renderer2D::GetRenderTargetSize().y;
		ImVec2 ImageSize = { windowSize.y * aspectRatio,windowSize.y };
		float scale = 0.0f;
		double innerAspectRatio = aspectRatio;
		;
		double outerAspectRatio = windowSize.x / windowSize.y;
		if (innerAspectRatio < outerAspectRatio) {
			scale = ((double)windowSize.y) / ((double)ImageSize.y);
		}
		else {
			scale = ((double)windowSize.x) / ((double)ImageSize.x);
		}


		m_ImageSize = { scale * ImageSize.x, scale * ImageSize.y };

	}


	void Panel_Viewport::DrawResolutionSelectable() {
		Ref<Scene> scene = ProjectManager::GetActiveScene();
		if (scene) {
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100);
			if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
			{
				for (int n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
					if (ImGui::Selectable(items[n], is_selected))
					{
						current_item = items[n];
						std::string curr = std::string(current_item);

						int x = std::stoi(curr.substr(0, curr.find('x')));
						int y = std::stoi(curr.substr(curr.find('x') + 1, curr.length()));
						Renderer2D::SetRenderSize(x,y);
						UpdateViewPortSize();
						
						if (is_selected) {
							ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
						}
					}
				}	
				ImGui::EndCombo();
			}
		}
	}

	void Panel_Viewport::DrawPlayButton() {
		Ref<Scene> scene = ProjectManager::GetActiveScene();
		if(scene){

			ImGui::SetCursorPos({ (ImGui::GetWindowSize().x - 16.f) * 0.5f, 32 });
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_PlayButton_CurrIcon->GetRendererID(), ImVec2((float)16, (float)16), ImVec2(0, 1), ImVec2(1, 0))) {
				if (!scene->IsPlaying()) {
					ProjectManager::SaveCurrentScene();
					scene->OnSceneStart();
					m_PlayButton_CurrIcon = m_PlayButton_StopIcon;
				}
				else {
					scene->OnSceneEnd();
					ProjectManager::OpenLastScene();
					m_PlayButton_CurrIcon = m_PlayButton_PlayIcon;
				}
			}
		}
	}

	void Panel_Viewport::DrawFrameBufferImage() {
		Ref<Scene> scene = ProjectManager::GetActiveScene();
		if (scene) {
			UpdateViewPortSize();
			ImVec2 windowSize = ImGui::GetContentRegionAvail();
			
			ImGui::SetCursorPosX((windowSize.x - m_ImageSize.x) * 0.5f);
			ImGui::SetCursorPosY(((windowSize.y - m_ImageSize.y) * 0.5f) + (ImGui::GetWindowHeight() - windowSize.y));
			uint32_t textureID = Renderer2D::GetOutputAsTextureId();
			ImGui::PushID("ViewportDragTargetZone");
			ImGui::Image((void*)(uint64_t)textureID, { m_ImageSize.x, m_ImageSize.y });
			if (ImGui::BeginDragDropTarget())
			{
				char* path;
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("fileitem");
				if (payload) {
					path = new char[payload->DataSize + 1];
					memcpy((char*)&path[0], payload->Data, payload->DataSize);
					std::filesystem::path file_path = path;
					if (file_path.extension().compare(".asset") == 0) {
						ObjectSerializer serializer = ObjectSerializer();
						serializer.Deserialze(path, ProjectManager::GetActiveScene());
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
		}
	}


	void DrawWarningNoOpenProject() {
		std::string warning_text = "SurfEngine Version ";
		warning_text += SE_VERSION_MAJOR;
		warning_text += ".";
		warning_text += SE_VERSION_MINOR;
		warning_text += "\nCreate / Open Project File * .surf";

		auto windowWidth = ImGui::GetWindowSize().x;
		auto windowHeight = ImGui::GetWindowSize().y;
		auto textWidth = ImGui::CalcTextSize(warning_text.c_str()).x;
		auto textHeight = ImGui::CalcTextSize(warning_text.c_str()).y;

		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
		ImGui::SetCursorPosY((windowHeight - textHeight) * 0.5f);
		ImGui::Text(warning_text.c_str());
		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
		if (ImGui::Button("New Project")) {
			ImGui::OpenPopup("New Project Creation");
		}
		ImGui::SameLine();
		if (ImGui::Button("Open Project")) {
			MenuManager::BeginDialogue_OpenProject();
		}
		MenuManager::DrawNewProjectPopup();
		
	}

	void DrawWarningNoOpenScene() {
		std::string warning_text = "SurfEngine Version ";
		warning_text += SE_VERSION_MAJOR;
		warning_text += ".";
		warning_text += SE_VERSION_MINOR;
		warning_text += "\nCreate / Open Scene File * .scene";

		auto windowWidth = ImGui::GetWindowSize().x;
		auto windowHeight = ImGui::GetWindowSize().y;
		auto textWidth = ImGui::CalcTextSize(warning_text.c_str()).x;
		auto textHeight = ImGui::CalcTextSize(warning_text.c_str()).y;

		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
		ImGui::SetCursorPosY((windowHeight - textHeight) * 0.5f);
		ImGui::Text(warning_text.c_str());
		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
		if (ImGui::Button("New Scene")) {
			ImGui::OpenPopup("New Scene Creation");

		}
		ImGui::SameLine();
		if (ImGui::Button("Open Scene")) {
			
		}
		MenuManager::DrawNewScenePopup();
	}

	void Panel_Viewport::OnImGuiRender() {

		//ViewPort
		if (ImGui::Begin("View Port", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse)) {
			m_IsSelected = ImGui::IsWindowFocused();
			
			DrawResolutionSelectable();
			DrawPlayButton();
			DrawFrameBufferImage();

			if (!ProjectManager::IsActiveProject()) {
				DrawWarningNoOpenProject();
			}
			else if (!ProjectManager::IsActiveScene()) {
				DrawWarningNoOpenScene();
			}
		}
		ImGui::End();
	}	
}