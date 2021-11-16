#pragma once
#pragma once
#include "Camera.h"
#include "MechEngine/Core/Timestep.h"
#include "MechEngine/Core/Input.h"

#include "MechEngine/Core/KeyCodes.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/quaternion.hpp>


namespace MechEngine {
	class OrthographicCamera : public Camera {
	public:
		OrthographicCamera() {
			SetProjection(glm::ortho(-1.92f * m_Zoom,1.92f * m_Zoom,-1.08f * m_Zoom,1.08f * m_Zoom,0.1f,1000.f));
			m_Transform.SetPosition({ 0.f,0.f,5.f });
		}

		void OnEvent(Event& event) override {
			if (event.GetEventType() == EventType::MouseScrolled) {
				MouseScrolledEvent& e = (MouseScrolledEvent&)event;
				m_Zoom -= (e.GetYOffset() * 0.25) * (m_Zoom * 0.15);
				m_Zoom = std::max(m_Zoom, 0.15f);
				SetProjection(glm::ortho(-1.92f * m_Zoom, 1.92f * m_Zoom, -1.08f * m_Zoom, 1.08f * m_Zoom, 0.1f, 1000.f));
			}
		}

		void OnUpdate(Timestep ts) override {
			glm::vec2 tempMousePosition = Input::GetMousePosition();;
			glm::vec3 pos = m_Transform.GetPosition();

			

			if (Input::IsKeyPressed(ME_KEY_W)) {
				pos.y -= 1.f * m_TranslateSpeed;
			}
			if (Input::IsKeyPressed(ME_KEY_A)) {
				pos.x -= 1.f * m_TranslateSpeed;
			}
			if (Input::IsKeyPressed(ME_KEY_S)) {
				pos.y += 1.f * m_TranslateSpeed;
			}
			if (Input::IsKeyPressed(ME_KEY_D)) {
				pos.x += 1.f * m_TranslateSpeed;
			}
			m_Transform.SetPosition(pos);

			UpdateView();
			m_TranslateSpeed = m_Zoom*0.05;
			m_OldMousePosition = tempMousePosition;
		}

		void UpdateView() {
			m_View = glm::translate(glm::mat4(1.0f), m_Transform.GetPosition());
			m_ProjectionView = m_Projection * glm::inverse(m_View);
		}

		void SetProjection(glm::mat4 projection) override {
			m_Projection = projection;
			UpdateView();
		}

		void SetZoom(float zoom) {
			m_Zoom = zoom;
			SetProjection(glm::ortho(-1.92f * m_Zoom, 1.92f * m_Zoom, -1.08f * m_Zoom, 1.08f * m_Zoom, 0.1f, 1000.f));
		}


	private:
		float m_Zoom = 1.0f;
		float m_TranslateSpeed = .05f;
		glm::vec2 m_OldMousePosition;
	};
}