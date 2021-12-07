#pragma once
#include "SurfEngine/Core/Timestep.h"
#include "SurfEngine/Events/Event.h"
#include <glm/glm.hpp>

namespace SurfEngine {

	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			: m_Projection(projection) {}

		virtual ~Camera() = default;

		virtual void SetProjection(glm::mat4 projection) {}

		const glm::mat4& GetProjection() const { return m_Projection; }
		const glm::mat4& GetView() const { return m_View; }
		void SetView(const glm::mat4& view) { m_View = view; m_ProjectionView = m_Projection * glm::inverse(m_View); }
		const glm::mat4& GetViewProjection() const { return m_ProjectionView; }

		virtual void OnEvent(Event& e) {}
		virtual void OnUpdate(Timestep ts) {}
	public:
		glm::mat4 m_Transform;
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
		glm::mat4 m_View = glm::mat4(1.0f);
		glm::mat4 m_ProjectionView = glm::mat4(1.0f);
	};

}