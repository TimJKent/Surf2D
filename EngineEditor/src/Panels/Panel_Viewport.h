#pragma once
#include "MechEngine/Core/Core.h"

#include <glm/glm.hpp>

namespace MechEngine {
	class Panel_Viewport {
	public:
		Panel_Viewport() {

		}

		void OnImGuiRender();

		void SetSelected(bool selected) { m_IsSelected = selected; }
		bool GetSelected() { return m_IsSelected; }

		glm::vec2 GetViewPortPosition() { return m_ViewPortPosition; }
		glm::vec2 GetViewPortSize() { return m_ViewPortSize; }

		void SetViewPortPosition(glm::vec2 pos) { m_ViewPortPosition = pos; }
		void SetViewPortSize(glm::vec2 size) { m_ViewPortSize = size; }


	private:
		bool m_IsSelected = false;
		glm::vec2 m_ViewPortSize = {1920.f,1080.f};
		glm::vec2 m_ViewPortPosition = {0.f,0.f};
	};
}


