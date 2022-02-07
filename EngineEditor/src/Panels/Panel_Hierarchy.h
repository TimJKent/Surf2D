#pragma once
#include "SurfEngine/Core/Core.h"
#include "SurfEngine/Scenes/Scene.h"
#include "SurfEngine/Scenes/Object.h"
#include "SurfEngine/Renderer/Texture.h"


namespace SurfEngine {
	class Panel_Hierarchy{
	public:
		Panel_Hierarchy();

		void OnImGuiRender();
	

	private:
		void DrawObjectNode(entt::entity object);
		Ref<Texture2D> add_icon;
		friend class Panel_Inspector;
	};
}