#pragma once
#include "RendererAPI.h"
namespace SurfEngine {
	class RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4 color) { s_RendererAPI->SetClearColor(color); }
		inline static void Clear() { s_RendererAPI->Clear(); }
		inline static void EnableTextures() { s_RendererAPI->EnableTextures(); }
		inline static void EnableMSAA() { s_RendererAPI->EnableMSAA(); }
		inline static void EnableDepthTesting() { s_RendererAPI->EnableDepth(); }
		inline static void EnableBlending() { s_RendererAPI->EnableBlending(); }
		inline static void BindTextureID(int slot, std::uint32_t id) { s_RendererAPI->BindTextureId(slot,id); }
		inline static void SetWireFrameMode(RendererAPI::WireFrameMode mode) { s_RendererAPI->SetWireFrameMode(mode); }
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray) { s_RendererAPI->DrawIndexed(vertexArray); }
		inline static void DrawIndexedPoints(const Ref<VertexArray>& vertexArray) {s_RendererAPI->DrawIndexedPoints(vertexArray);}
	private:
		static RendererAPI* s_RendererAPI;
	};
}

