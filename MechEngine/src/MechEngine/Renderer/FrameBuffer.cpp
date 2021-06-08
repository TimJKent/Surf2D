#include "mepch.h"
#include "FrameBuffer.h"
#include "Renderer.h"
#include "MechEngine/Platform/OpenGl/OpenGLFrameBuffer.h"

namespace MechEngine{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec) {
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None:	ME_CORE_ASSERT(false, "Renderer API not supported"); return nullptr;
			case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLFramebuffer>(spec);
		}
		ME_CORE_ASSERT(false, "Unknown Renderer API Specified");
		return nullptr;
	}
}
