#pragma once
#include "Camera.h"
#include "MechEngine/Core/Timestep.h"
#include "MechEngine/Core/Input.h"

#include "MechEngine/Core/KeyCodes.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/quaternion.hpp>

#include "MechEngine/Core/Transform.h"

namespace MechEngine {
	class PerspectiveCamera : public Camera {
	public:
		PerspectiveCamera() { 
			SetProjection(glm::perspective(glm::radians(66.f),1.77f, 0.1f,1000.f));
		}
		
		void OnUpdate(Timestep ts) override {
			glm::vec2 tempMousePosition = Input::GetMousePosition();;
			glm::vec3 pos = m_Transform.GetPosition();
			
			if (Input::IsMouseButtonPressed(ME_MOUSE_BUTTON_3)) {
					m_Transform.Rotate({ 0.0f, (tempMousePosition.x - m_OldMousePosition.x) * ts * m_RotationSpeed,0.0f });
					m_Transform.Rotate({ -(tempMousePosition.y - m_OldMousePosition.y) * ts * m_RotationSpeed, 0.f ,0.0f });
					float x_rot = std::clamp(m_Transform.GetRotation().x, -85.f, 85.f);
					float y_rot = m_Transform.GetRotation().y;
					float z_rot = m_Transform.GetRotation().z;
					m_Transform.SetRotation({ x_rot, y_rot, z_rot });
			}

			if (Input::IsKeyPressed(ME_KEY_W)) {
				pos -= m_Transform.Forward() * ts.GetSeconds() * m_TranslateSpeed;
			}
			if (Input::IsKeyPressed(ME_KEY_A)) {
				pos -= m_Transform.Right() * ts.GetSeconds() * m_TranslateSpeed;
			}
			if (Input::IsKeyPressed(ME_KEY_S)) {
				pos += m_Transform.Forward() * ts.GetSeconds() * m_TranslateSpeed;
			}
			if (Input::IsKeyPressed(ME_KEY_D)) {
				pos += m_Transform.Right() * ts.GetSeconds() * m_TranslateSpeed;
			}
			m_Transform.SetPosition(pos);
			
			UpdateView();
			m_OldMousePosition = tempMousePosition;
		}

		void UpdateView() {
			m_View = glm::translate(glm::mat4(1.0f), m_Transform.GetPosition()) * glm::toMat4(m_Transform.GetOrientation());
			m_ProjectionView = m_Projection * glm::inverse(m_View);
		}

		void SetProjection(glm::mat4 projection) override {
			m_Projection = projection;
			UpdateView();
		}


	private:
		float m_RotationSpeed  = 15.f;
		float m_TranslateSpeed = 2.5f;
		glm::vec2 m_OldMousePosition;
	};
}