#pragma once
#include "Camera.h"
#include "RenderCommand.h"
#include "RendererAPI.h"
#include "Shader.h"

namespace MechEngine {


	class Renderer {
	public:
		static void BeginScene(Camera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader> shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));



		static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData {
			glm::mat4 ViewProjectionMatrix;
		};
		static SceneData* m_SceneData;
	};

}


