#pragma once
#include <glm/glm.hpp>
#include "VertexArray.h"

namespace SurfEngine {

	class RendererAPI {
	public:
		enum class API {
			None =0, OpenGL = 1
		};
		enum class WireFrameMode {
			Off = 0, On = 1
		};


	public:
		virtual void SetClearColor(const glm::vec4 color) = 0;
		virtual void Clear() = 0;
		virtual void EnableTextures() = 0;
		virtual void EnableMSAA() = 0;
		virtual void EnableDepth() = 0;
		virtual void EnableBlending() = 0;
		virtual void SetWireFrameMode(WireFrameMode mode) = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;
		virtual void DrawIndexedPoints(const Ref<VertexArray>& vertexArray) = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}