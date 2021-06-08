#pragma once
#include "MechEngine/Renderer/Camera.h"
#include "MechEngine/Core/Timestep.h"
#include "MechEngine/Events/ApplicationEvent.h"
#include "MechEngine/Events/MouseEvent.h"
#include "MechEngine/Core/Transform.h"

namespace MechEngine {
	class CameraController
	{
	public:
		Transform transform;
	public:
		virtual Camera& GetCamera() = 0;
		virtual const Camera& GetCamera() const = 0;
		virtual void SetZoom(float zoom) {};
		const float GetZoom() const { return m_Zoom; }

		virtual void OnUpdate(Timestep ts) {}
		virtual void OnEvent(Event& e) {}
	protected:
		virtual bool OnMouseScrolled(MouseScrolledEvent& e) { return false; }
		virtual bool OnWindowResized(WindowResizeEvent& e) { return false; }
	protected:
		float m_Zoom = 1.0f;
	};
}

