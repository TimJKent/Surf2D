#include "mepch.h"
#include "RenderCommand.h"

#include "MechEngine/Platform/OpenGl/OpenGLRendererAPI.h"

namespace MechEngine {
	 RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}