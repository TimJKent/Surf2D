#pragma once
#include "SurfEngine/Core/Core.h"
#include "SurfEngine/Scenes/Scene.h"
#include "SurfEngine/Scenes/Object.h"


namespace SurfEngine {
	class Panel_Hierarchy{
	public:
		Panel_Hierarchy();

		void OnImGuiRender();
		
		Object GetSelectedObject();
		void SetSelectedObject(Object object);

	private:
		Object m_SelectedObjectContext;
	private:
		void DrawObjectNode(Object object);
		friend class Panel_Inspector;
	};
}