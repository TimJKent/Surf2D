#include "sepch.h"

#include "Panel_Viewport.h"
#include "ProjectManager.h"
#include "SurfEngine/Renderer/Renderer2D.h"




namespace SurfEngine{


	Panel_Viewport::Panel_Viewport() {
		m_PlayButton_PlayIcon = Texture2D::Create("res\\textures\\icon_play.png");
		m_PlayButton_StopIcon = Texture2D::Create("res\\textures\\icon_stop.png");
		m_PlayButton_CurrIcon = m_PlayButton_PlayIcon;
	}

	void Panel_Viewport::DrawPlayButton() {
		Ref<Scene> scene = ProjectManager::GetActiveScene();
		if(scene){
			ImGui::SetCursorPos({ (ImGui::GetWindowSize().x - 16.f) * 0.5f, 32 });
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_PlayButton_CurrIcon->GetRendererID(), ImVec2((float)16, (float)16), ImVec2(0, 1), ImVec2(1, 0))) {
				if (!scene->IsPlaying()) {
					scene->OnSceneStart();
					ProjectManager::SaveCurrentScene();
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
			if (scene->GetSceneCamera()) {
				ImVec2 windowSize = ImGui::GetContentRegionAvail();
				if ((windowSize.x != m_ViewPortSize.x) || (windowSize.y != m_ViewPortSize.y)) {
					m_ViewPortSize = windowSize;
					float aspectRatio = scene->GetSceneCamera()->GetAspectRatio();
					ImVec2 ImageSize = { windowSize.y * aspectRatio,windowSize.y };
					float scale = 0.0f;
					double innerAspectRatio = aspectRatio;
					double outerAspectRatio = windowSize.x / windowSize.y;
					if (innerAspectRatio < outerAspectRatio) {
						scale = ((double)windowSize.y) / ((double)ImageSize.y);
					}
					else {
						scale = ((double)windowSize.x) / ((double)ImageSize.x);
					}

					
					m_ImageSize = { scale * ImageSize.x, scale * ImageSize.y };
					
					Renderer2D::ResizeRenderTarget((uint32_t)m_ImageSize.x, (uint32_t)m_ImageSize.y);
				}
				
				ImGui::SetCursorPosX((windowSize.x - m_ImageSize.x) * 0.5f);
				ImGui::SetCursorPosY(((windowSize.y - m_ImageSize.y) * 0.5f) + (ImGui::GetWindowHeight() - windowSize.y));
				uint32_t textureID = Renderer2D::GetOutputAsTextureId();
				ImGui::Image((void*)(uint64_t)textureID, m_ImageSize);
			}	
		}
	}

	void DrawWarningText(const std::string& warning_text) {
			auto windowWidth = ImGui::GetWindowSize().x;
			auto windowHeight = ImGui::GetWindowSize().y;
			auto textWidth = ImGui::CalcTextSize(warning_text.c_str()).x;
			auto textHeight = ImGui::CalcTextSize(warning_text.c_str()).y;

			ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
			ImGui::SetCursorPosY((windowHeight - textHeight) * 0.5f);
			ImGui::Text(warning_text.c_str());
			ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	}

	void Panel_Viewport::OnImGuiRender() {
		//ViewPort
	    m_IsAspectRatioChanged = false;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(500.f, 500.f));
		if (ImGui::Begin("View Port", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse)) {
			m_IsSelected = ImGui::IsWindowFocused();

			DrawPlayButton();
			DrawFrameBufferImage();

			if (!ProjectManager::IsActiveProject()) {
				std::string outText = "SurfEngine Version ";
				outText += SE_VERSION_MAJOR;
				outText += ".";
				outText += SE_VERSION_MINOR;
				outText += "\nCreate / Open Project File * .surf";
				DrawWarningText(outText);
			}
			else if (!ProjectManager::IsActiveScene()) {
				std::string outText = "SurfEngine Version ";
				outText += SE_VERSION_MAJOR;
				outText += ".";
				outText += SE_VERSION_MINOR;
				outText += "\nCreate / Open Scene File * .scene";
				DrawWarningText(outText);
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}	
}