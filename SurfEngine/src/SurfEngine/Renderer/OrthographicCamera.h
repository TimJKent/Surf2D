#pragma once
#include "Camera.h"
#include "SurfEngine/Core/Timestep.h"
#include "SurfEngine/Core/Input.h"

#include "SurfEngine/Core/KeyCodes.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/quaternion.hpp>


namespace SurfEngine {
	class OrthographicCamera : public Camera {
	public:
		OrthographicCamera() {
			SetProjection(glm::ortho(-1.92f * m_Zoom, 1.92f * m_Zoom, -1.08f * m_Zoom, 1.08f * m_Zoom, 0.1f, 1000.f));
			m_Transform = glm::mat4(1.0f);
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
			glm::vec2 tempMousePosition = Input::GetMousePosition();

			

			if (Input::IsKeyPressed(SE_KEY_W)) {
				m_Transform = glm::translate(m_Transform, glm::vec3{ 0.f,-1.f* m_TranslateSpeed ,0.f });
			}
			if (Input::IsKeyPressed(SE_KEY_A)) {
				m_Transform = glm::translate(m_Transform, glm::vec3{ -1.f * m_TranslateSpeed, 0.f ,0.f });

			}
			if (Input::IsKeyPressed(SE_KEY_S)) {
				m_Transform = glm::translate(m_Transform, glm::vec3{ 0.f,1.f * m_TranslateSpeed ,0.f });

			}
			if (Input::IsKeyPressed(SE_KEY_D)) {
				m_Transform = glm::translate(m_Transform, glm::vec3{ 1.f * m_TranslateSpeed, 0.f ,0.f });

			}

			UpdateView();
			m_TranslateSpeed = m_Zoom*0.05;
			m_OldMousePosition = tempMousePosition;
		}

		void UpdateView() {
			m_View = m_Transform;
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