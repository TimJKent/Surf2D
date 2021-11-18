#include "sepch.h"
#include "Renderer.h"
#include "Camera.h"
#include "SurfEngine/Platform/OpenGl/OpenGLShader.h"

namespace SurfEngine {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::BeginScene(Camera& camera) {
		m_SceneData->ViewProjectionMatrix = camera.GetProjection();
	}

	void Renderer::EndScene(){
	
	}

	void Renderer::Submit(const Ref<Shader> shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform){
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection",m_SceneData->ViewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}