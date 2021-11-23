#include "sepch.h"

#include "Panel_Viewport.h"
#include "SurfEngine/Renderer/Renderer2D.h"


#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace SurfEngine{

	void Panel_Viewport::OnImGuiRender() {
		//ViewPort
		float ratio = 0.5625f;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(500.f	, 500.f));
		if (ImGui::Begin("ViewPort",NULL,ImGuiWindowFlags_NoScrollbar)) {

			if (ImGui::BeginDragDropTarget())
			{

				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("window");
				ImGui::EndDragDropTarget();
			}

			m_IsSelected = ImGui::IsWindowFocused();
			ImVec2 viewPortSize = ImGui::GetContentRegionAvail();
			if (viewPortSize.x != m_ViewPortSize.x || viewPortSize.y != m_ViewPortSize.y) {
				m_ViewPortSize = { viewPortSize.x, viewPortSize.y };
				Renderer2D::ResizeRenderTarget(m_ViewPortSize.x, m_ViewPortSize.y);
			}
			//Ask our fbo to put its output into a texture
			uint32_t textureID = Renderer2D::GetOutputAsTextureId();
			//clamp its vertical height to the renderer asepect ratio - important to prevent distortion
			ImVec2 size;
			if ((m_ViewPortSize.x * ratio - ImGui::GetContentRegionAvail().y)>0) {
				size = ImVec2(m_ViewPortSize.y*1.77, m_ViewPortSize.y);
			}
			else {
				size = ImVec2(m_ViewPortSize.x, m_ViewPortSize.x * ratio);
			}
			//Render the clamped image onto the window
			ImGui::Image((void*)textureID, size);
		}
		ImGui::End();
		ImGui::PopStyleVar();

	}
	
}