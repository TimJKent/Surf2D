#pragma once
#include "SurfEngine/Core/Layer.h"
#include "SurfEngine/Events/KeyEvent.h"
#include "SurfEngine/Events/MouseEvent.h"
#include "SurfEngine/Events/ApplicationEvent.h"

namespace SurfEngine {
	class SurfEngine_API ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
		void Begin();
		void End();
	private:
		float m_Time;
	};
}
