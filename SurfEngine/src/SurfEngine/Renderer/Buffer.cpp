#include "sepch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "SurfEngine/Platform/OpenGl/OpenGLBuffer.h"

namespace SurfEngine {
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size) {
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None: SE_CORE_ASSERT(false, "Renderer API not supported"); return nullptr;
			case RendererAPI::API::OpenGL: return new OpenGLVertexBuffer(vertices,size);
		}
		SE_CORE_ASSERT(false, "Unknown Renderer API Specified");
		return nullptr;
	} 

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size) {
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None: SE_CORE_ASSERT(false, "Renderer API not supported"); return nullptr;
			case RendererAPI::API::OpenGL: return new OpenGLIndexBuffer(indices, size);
		}
		SE_CORE_ASSERT(false, "Unknown Renderer API Specified");
		return nullptr;
	}
}