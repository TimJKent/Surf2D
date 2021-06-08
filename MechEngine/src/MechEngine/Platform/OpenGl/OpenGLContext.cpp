#include "mepch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace MechEngine {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle){
		ME_CORE_ASSERT(windowHandle, "Window Handle is null");
	}

	void OpenGLContext::Init(){
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ME_CORE_ASSERT(status, "Failed to initilized Glad!");

		ME_CORE_INFO("	Vendor: {0}",   glGetString(GL_VENDOR));
		ME_CORE_INFO("	Renderer: {0}", glGetString(GL_RENDERER));
		ME_CORE_INFO("	Version: {0}",  glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers(){
		glfwSwapBuffers(m_WindowHandle);
	}
}