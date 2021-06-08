#pragma once
#include "MechEngine/Layer.h"
#include "MechEngine/Events/KeyEvent.h"
#include "MechEngine/Events/MouseEvent.h"
#include "MechEngine/Events/ApplicationEvent.h"

namespace MechEngine {
	class MECHENGINE_API ImGuiLayer : public Layer {
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
