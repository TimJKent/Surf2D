#include "mepch.h"

#include "Panel_Viewport.h"
#include "MechEngine/Renderer/Renderer2D.h"


#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace MechEngine{

	void Panel_Viewport::OnImGuiRender() {
		//ViewPort
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f,0.0f });
		if (ImGui::Begin("ViewPort")) {
			m_IsSelected = ImGui::IsWindowFocused();
			ImVec2 viewPortSize = ImGui::GetContentRegionAvail();
			if (viewPortSize.x != m_ViewPortSize.x || viewPortSize.y != m_ViewPortSize.y) {
				m_ViewPortSize = { viewPortSize.x, viewPortSize.y };
				Renderer2D::ResizeRenderTarget(m_ViewPortSize.x, m_ViewPortSize.y);
			}

			//Ask our fbo to put its output into a texture
			uint32_t textureID = Renderer2D::GetOutputAsTextureId();
			//clamp its vertical height to the renderer asepect ratio - important to prevent distortion
			float ratio = 0.5625f;
			ImVec2 size = ImVec2(m_ViewPortSize.x, m_ViewPortSize.x * ratio);
			//Render the clamped image onto the window
			ImGui::Image((void*)textureID, size);
		}
		ImGui::End();
		ImGui::PopStyleVar();

	}
	
}