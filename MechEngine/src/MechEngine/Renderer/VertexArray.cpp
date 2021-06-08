#include "mepch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "MechEngine/Platform/OpenGl/OpenGLVertexArray.h"
#include "MechEngine/Platform/OpenGl/OpenGLBuffer.h"

namespace MechEngine {
	Ref<VertexArray> VertexArray::Create() {
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None: ME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexArray>();
		}
		ME_CORE_ASSERT(false, "Unknown RendererAPI specified!");
		return nullptr;
	}
}

