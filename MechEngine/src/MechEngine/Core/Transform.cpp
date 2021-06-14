#include "mepch.h"
#include "Transform.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <glm/gtx/quaternion.hpp>


namespace MechEngine{
	Transform::Transform(){}

	const glm::vec3 Transform::GetPosition()	const {
		return m_Position;
	}

	const glm::vec3 Transform::GetScale()		const {
		return m_Scale;
	}

	const glm::vec3 Transform::GetRotation()	const{
		return m_Rotation;
	}

	void Transform::SetPosition(glm::vec3 position){
		m_Position = position;
	}

	void Transform::SetPosition(glm::vec2 position) {
		m_Position = { position.x, position.y, 1.f };
	}

	void Transform::SetScale(glm::vec3 scale){
		m_Scale = scale;
	}

	void Transform::SetRotation(glm::vec3 rotation){
		m_Rotation = rotation;
	}

	void Transform::Translate(glm::vec3 deltaPosition){
		m_Position += deltaPosition;
	}

	void Transform::Translate(float x, float y, float z){
		m_Position.x += x;
		m_Position.y += y;
		m_Position.z += z;
	}

	void Transform::Rotate(glm::vec3 deltaRotation){
		m_Rotation += deltaRotation;
	}

	void Transform::Rotate(float x, float y, float z){
		m_Rotation.x += x;
		m_Rotation.y += y;
		m_Rotation.z += z;
	}

	glm::vec3 Transform::Right()const {
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 Transform::Up()const {
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 Transform::Forward() const{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	glm::quat Transform::GetOrientation() const
	{
		return glm::quat(glm::vec3(-glm::radians(m_Rotation.x), -glm::radians(m_Rotation.y), 0.0f));
	}


	const glm::mat4 Transform::GetTransformMatrix() const {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), {m_Position.x,-m_Position.y,m_Position.z}) * glm::scale(glm::mat4(1.0f), { m_Scale.x,  m_Scale.y , m_Scale.z });
		transform = glm::rotate(transform, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		transform = glm::rotate(transform, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		transform = glm::rotate(transform, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		return transform;
	}
}