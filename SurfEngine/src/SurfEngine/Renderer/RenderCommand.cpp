#include "sepch.h"
#include "RenderCommand.h"

#include "SurfEngine/Platform/OpenGl/OpenGLRendererAPI.h"

namespace SurfEngine {
	 RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}