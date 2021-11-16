#pragma once
#include "MechEngine/Core/Core.h"
#include "MechEngine/Scenes/Scene.h"
#include "MechEngine/Scenes/Object.h"


namespace MechEngine {
	class Panel_Hierarchy{
	public:
		Panel_Hierarchy() = default;
		Panel_Hierarchy(const Ref<Scene>& scene);

		void SetActiveScene(Ref<Scene>& scene);

		void OnImGuiRender();
		
		Object GetSelectedObject();
		void SetSelectedObject(Object object);

	private:
		Ref<Scene> m_ActiveScene;
		Object m_SelectedObjectContext;
	private:
		void DrawObjectNode(Object object);
		friend class Panel_Inspector;
	};
}