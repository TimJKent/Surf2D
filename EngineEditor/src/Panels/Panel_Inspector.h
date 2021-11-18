#pragma once
#include "SurfEngine/Core/Core.h"
#include "SurfEngine/Scenes/Scene.h"
#include "SurfEngine/Scenes/Object.h"
#include "Panel_Hierarchy.h"


namespace SurfEngine {
	class Panel_Inspector {
	public:
		Panel_Inspector(Ref<Panel_Hierarchy>& hierarchy) {
			m_panel_hierarchy = hierarchy;
		}

		void OnImGuiRender();

		void SetDebugMode(bool debugMode) { m_DebugMode = debugMode; }
		bool GetDebugMode() { return m_DebugMode; }

	private:
		Ref<Panel_Hierarchy> m_panel_hierarchy;
		bool m_DebugMode = false;
	private:
		void DrawComponentTag(Object);
		void DrawComponentTransform(Object);
		void DrawComponentSpriteRenderer(Object);
	};
}