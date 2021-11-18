#include "sepch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace SurfEngine {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle){
		SE_CORE_ASSERT(windowHandle, "Window Handle is null");
	}

	void OpenGLContext::Init(){
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		SE_CORE_ASSERT(status, "Failed to initilized Glad!");

		SE_CORE_INFO("	Vendor: {0}",   glGetString(GL_VENDOR));
		SE_CORE_INFO("	Renderer: {0}", glGetString(GL_RENDERER));
		SE_CORE_INFO("	Version: {0}",  glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers(){
		glfwSwapBuffers(m_WindowHandle);
	}
}