#pragma once
#include "SurfEngine/Renderer/RendererAPI.h"


namespace SurfEngine {
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void SetClearColor(const glm::vec4 color) override;
		virtual void Clear() override;
		virtual void EnableTextures() override;
		virtual void EnableMSAA() override;
		virtual void EnableDepth() override;
		virtual void BindTextureId(int slot, uint32_t id) override;
		virtual void EnableBlending() override;
		virtual void SetWireFrameMode(RendererAPI::WireFrameMode mode) override;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
		virtual void DrawIndexedPoints(const Ref<VertexArray>& vertexArray) override;

	};
}

