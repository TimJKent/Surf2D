#pragma once
#include "glm/glm.hpp"

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

	private:
		glm::vec3 m_Position = {0.0f,0.0f,0.0f};
		glm::vec3 m_Rotation = {0.0f,0.0f,0.0f};
		glm::vec3 m_Scale = {1.0f,1.0f,1.0f};
	};
}