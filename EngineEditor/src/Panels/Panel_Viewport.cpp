#include "sepch.h"

#include "Panel_Viewport.h"
#include "ProjectManager.h"
#include "SurfEngine/Renderer/Renderer2D.h"


#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace SurfEngine{


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
		float ratio = 0.5625f;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(500.f, 500.f));
		if (ImGui::Begin("View Port", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse)) {
			m_IsSelected = ImGui::IsWindowFocused();
			ImVec2 viewPortSize = ImGui::GetContentRegionAvail();
			if (viewPortSize.x != m_ViewPortSize.x || viewPortSize.y != m_ViewPortSize.y) {
				m_ViewPortSize = { viewPortSize.x, viewPortSize.y };
				Renderer2D::ResizeRenderTarget((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			}
			//Ask our fbo to put its output into a texture
			uint32_t textureID = Renderer2D::GetOutputAsTextureId();
			//clamp its vertical height to the renderer asepect ratio - important to prevent distortion
			ImVec2 size = ImGui::GetContentRegionAvail();
			ImGui::SetCursorPos({ (ImGui::GetWindowSize().x - 16.f) * 0.5f, 32});
			auto& scene = ProjectManager::GetActiveScene();
			if (scene) {
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

				ImGui::Image((void*)(uint64_t)textureID, { size.x, size.y - 32 });
			}
			else if (ProjectManager::IsActiveProject()) {
				DrawWarningText("SurfEngine Version 1.0\nCreate/Open Scene file *.scene");
			}
			else {
				DrawWarningText("SurfEngine Version 1.0\nCreate/Open Project file *.surf");
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}	
}