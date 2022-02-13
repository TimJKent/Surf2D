#include "sepch.h"
#include "SceneCamera.h"
#include "SurfEngine/Renderer/Renderer2D.h"

#include <glm/gtc/matrix_transform.hpp>

namespace SurfEngine{

	SceneCamera::SceneCamera()
	{
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
	}

	
	
	void SceneCamera::RecalculateProjection()
	{
			glm::vec2 renderSize = Renderer2D::GetRenderTargetSize();
			UpdateView();
			float orthoLeft = -m_OrthographicSize * (renderSize.x / renderSize.y) * 0.5f;
			float orthoRight = m_OrthographicSize * (renderSize.x / renderSize.y) * 0.5f;
			float orthoBottom = -m_OrthographicSize * 0.5f;
			float orthoTop = m_OrthographicSize * 0.5f;

			m_Projection = glm::ortho(orthoLeft, orthoRight,
				orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);

	}

}
