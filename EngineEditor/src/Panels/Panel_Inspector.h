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
		void DrawComponentTag(std::vector<Ref<Object>> objects);
		void DrawComponentTransform(std::vector<Ref<Object>> objects);
		void DrawComponentSpriteRenderer(std::vector<Ref<Object>> objects);
		void DrawComponentAnimation(std::vector<Ref<Object>> objects);
		void DrawComponentCamera(std::vector<Ref<Object>> objects);
		void DrawComponentScript(std::vector<Ref<Object>> objects);
		void DrawComponentBoxCollider(std::vector<Ref<Object>> objects);
		void DrawComponentCircleCollider(std::vector<Ref<Object>> objects);
		void DrawComponentRigidBody(std::vector<Ref<Object>> objects);
	};
}