#pragma once
#include "SurfEngine/Core/Core.h"
#include "SurfEngine/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace SurfEngine {
	class Panel_Viewport {
	public:
		Panel_Viewport() {
			m_PlayButton_PlayIcon = Texture2D::Create("res\\textures\\icon_play.png");
			m_PlayButton_StopIcon = Texture2D::Create("res\\textures\\icon_stop.png");
			m_PlayButton_CurrIcon = m_PlayButton_PlayIcon;
		}

		void OnImGuiRender();

		bool GetSelected() { return m_IsSelected; }

		glm::vec2 GetViewPortPosition() { return m_ViewPortPosition; }
		glm::vec2 GetViewPortSize() { return m_ViewPortSize; }

		void SetViewPortPosition(glm::vec2 pos) { m_ViewPortPosition = pos; }
		void SetViewPortSize(glm::vec2 size) { m_ViewPortSize = size; }


	private:
		Ref<Texture2D> m_PlayButton_PlayIcon;
		Ref<Texture2D> m_PlayButton_StopIcon;
		Ref<Texture2D> m_PlayButton_CurrIcon;
		bool m_IsSelected = false;
		glm::vec2 m_ViewPortSize = {1920.f,1080.f};
		glm::vec2 m_ViewPortPosition = {0.f,0.f};
	};
}


