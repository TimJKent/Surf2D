#pragma once

#include "SurfEngine/Renderer/Camera.h"

namespace SurfEngine {

	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);


		float GetOrthographicSize() const { return m_OrthographicSize; }
		void  SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }

		float GetOrthographicNearClip() const { return m_OrthographicNear; }
		void  SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); }

		float GetOrthographicFarClip() const { return m_OrthographicFar; }
		void  SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); }

		float GetAspectRatio() const { return m_AspectRatio; }
		void  SetAspectRatio(float aspect) { m_AspectRatio = aspect; RecalculateProjection(); }

	protected:
		void RecalculateProjection();
	private:

		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 100.0f;
		float m_AspectRatio = 1.76f;
	};
}
