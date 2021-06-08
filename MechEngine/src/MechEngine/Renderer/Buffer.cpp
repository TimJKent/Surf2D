#include "mepch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "MechEngine/Platform/OpenGl/OpenGLBuffer.h"

namespace MechEngine {
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size) {
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None: ME_CORE_ASSERT(false, "Renderer API not supported"); return nullptr;
			case RendererAPI::API::OpenGL: return new OpenGLVertexBuffer(vertices,size);
		}
		ME_CORE_ASSERT(false, "Unknown Renderer API Specified");
		return nullptr;
	} 

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size) {
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None: ME_CORE_ASSERT(false, "Renderer API not supported"); return nullptr;
			case RendererAPI::API::OpenGL: return new OpenGLIndexBuffer(indices, size);
		}
		ME_CORE_ASSERT(false, "Unknown Renderer API Specified");
		return nullptr;
	}
}