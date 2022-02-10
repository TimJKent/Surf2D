#pragma once
#include "SurfEngine/Core/Core.h"
#include "SurfEngine/Scenes/Scene.h"
#include "SurfEngine/Renderer/Texture.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


namespace SurfEngine {
	class Panel_Viewport {
	public:
		Panel_Viewport();

		void OnImGuiRender();

		bool GetSelected() { return m_IsSelected; }

		bool ChangedAspectRatio() {
			return m_IsAspectRatioChanged;
		}
		
		float GetWidth() {
			return m_ViewPortSize.x;
		}

		float GetHeight() {
			return m_ViewPortSize.y;
		}
	private:
		void DrawPlayButton();
		void DrawFrameBufferImage();

	private:
		Ref<Texture2D> m_PlayButton_PlayIcon;
		Ref<Texture2D> m_PlayButton_StopIcon;
		Ref<Texture2D> m_PlayButton_CurrIcon;
		ImVec2 m_ViewPortSize = ImVec2(0,0 );
		ImVec2 m_ImageSize = ImVec2(0,0);
		bool m_IsAspectRatioChanged = false;
		bool m_IsSelected = false;
	};
}


