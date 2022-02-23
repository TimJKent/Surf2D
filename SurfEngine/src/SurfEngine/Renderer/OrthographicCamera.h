#pragma once
#include "Camera.h"
#include "SurfEngine/Core/Timestep.h"
#include "SurfEngine/Core/Input.h"
#include "SurfEngine/Scenes/SceneCamera.h"
#include "SurfEngine/Events/MouseEvent.h"
#include "SurfEngine/Core/KeyCodes.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/quaternion.hpp>


namespace SurfEngine {
	class OrthographicCamera : public SceneCamera {
	public:
		OrthographicCamera() {
			m_Transform = glm::mat4(1.0f);
			m_Transform = glm::translate(m_Transform, glm::vec3{ 0.f,0.f,0.0f });
			
			SceneCamera();
		}

		void OnEvent(Event& event) override {
			if (event.GetEventType() == EventType::MouseScrolled) {
				MouseScrolledEvent& e = (MouseScrolledEvent&)event;
				SetOrthographicSize(std::max(GetOrthographicSize() - ((e.GetYOffset() * 0.25f) * (GetOrthographicSize() * 0.15f)), 0.15f));
				RecalculateProjection();
			}
		}

		void OnUpdate(Timestep ts) override {
			if (Input::IsKeyPressed(SE_KEY_W)) {
				m_Transform = glm::translate(m_Transform, glm::vec3{ 0.f,-1.f * m_TranslateSpeed * ts ,0.f });
			}
			if (Input::IsKeyPressed(SE_KEY_A)) {
				m_Transform = glm::translate(m_Transform, glm::vec3{ -1.f * m_TranslateSpeed * ts, 0.f ,0.f });

			}
			if (Input::IsKeyPressed(SE_KEY_S)) {
				m_Transform = glm::translate(m_Transform, glm::vec3{ 0.f,1.f * m_TranslateSpeed * ts,0.f });

			}
			if (Input::IsKeyPressed(SE_KEY_D)) {
				m_Transform = glm::translate(m_Transform, glm::vec3{ 1.f * m_TranslateSpeed * ts, 0.f ,0.f });

			}
		
			m_TranslateSpeed = GetOrthographicSize() * 0.5f;
			RecalculateProjection();
		}


	private:
		float m_TranslateSpeed = .05f;
	};
}