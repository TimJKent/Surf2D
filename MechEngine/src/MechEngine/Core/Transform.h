#pragma once
#include "glm/glm.hpp"
#include "MechEngine/Core/Serialization.h"

namespace MechEngine {
	
	class Transform {
	public:
		Transform();

		const glm::vec3 GetPosition()	const ;
		const glm::vec3 GetScale()		const ;
		const glm::vec3 GetRotation()	const ;

		void SetPosition(glm::vec3 position);
		void SetPosition(glm::vec2 position);
		void SetScale(glm::vec3 scale);

		void SetRotation(glm::vec3 rotation);

		void Translate(glm::vec3 deltaPosition);

		void Translate(float x, float y, float z);

		void Rotate(glm::vec3 deltaPosition);
		void Rotate(float x, float y, float z);

		const glm::mat4 GetTransformMatrix() const;

		glm::vec3 Right() const;
		glm::vec3 Up() const;
		glm::vec3 Forward() const;

		glm::quat GetOrientation() const;
	public: //Serialization Formating
		
		const void SERIAL_WRITE() {
			if (!Serialization::ReadyForWrite()) {
				ME_ERROR("ERROR - Transform: Serializer not ready for Write");
				return;
			}
			fprintf_s(Serialization::GetFile(), FORMAT_OUT(), m_Position.x, m_Position.y, m_Position.z, m_Rotation.x, m_Rotation.y, m_Rotation.z, m_Scale.x, m_Scale.y, m_Scale.z);
		}

		const void SERIAL_READ() {
			if (!Serialization::ReadyForRead()) {
				ME_ERROR("ERROR - Transform: Serializer not ready for Read");
				return;
			}
			fscanf_s(Serialization::GetFile(), FORMAT_IN(), &m_Position.x, &m_Position.y, &m_Position.z, &m_Rotation.x, &m_Rotation.y, &m_Rotation.z, &m_Scale.x, &m_Scale.y, &m_Scale.z);
		}

	private:
		glm::vec3 m_Position = {0.0f,0.0f,0.0f};
		glm::vec3 m_Rotation = {0.0f,0.0f,0.0f};
		glm::vec3 m_Scale = {1.0f,1.0f,1.0f};

		const char* FORMAT_OUT() {
			return "(%f, %f, %f, %f, %f, %f, %f, %f, %f)\n";
		}

		const char* FORMAT_IN() {
			return "(%f, %f, %f, %f, %f, %f, %f, %f, %f)\n";
		}
	};
}