#include "sepch.h"
#include "FrameBuffer.h"
#include "Renderer.h"
#include "SurfEngine/Platform/OpenGl/OpenGLFrameBuffer.h"

namespace SurfEngine{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec) {
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None:	SE_CORE_ASSERT(false, "Renderer API not supported"); return nullptr;
			case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLFramebuffer>(spec);
		}
		SE_CORE_ASSERT(false, "Unknown Renderer API Specified");
		return nullptr;
	}
}
