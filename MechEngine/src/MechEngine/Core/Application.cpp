#include "mepch.h"
#include "Application.h"
#include "Layer.h"
#include "MechEngine/Core/Input.h"
#include "MechEngine/Renderer/Renderer.h"

#include <GLFW/glfw3.h>

namespace MechEngine {

#define DEBUG_LOG false
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

	Application::Application(){
		ME_CORE_ASSERT(!s_Instance, "Application already exists");
		s_Instance = this;
		m_Window = Window::Create();
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application(){

	}

	void Application::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		if(DEBUG_LOG)
			ME_CORE_TRACE("{0}", e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
			(*--it)->OnEvent(e);
			if (e.Handled) {
				break;
			}
		}
	}

	

	void Application::Run() {
		
		while (m_Runing) {
			float time = (float)glfwGetTime(); // Platform::GetTime
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(timestep);
			
			//Render ImGui
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack) {
				if (!(m_Window->GetWidth() <= 0 || m_Window->GetHeight() <= 0)) {
				   layer->OnImGuiRender();
				}
				
			}
			m_ImGuiLayer->End();
		
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Runing = false;
		return true;
	}

	void Application::PushLayer(Layer* layer) {
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay) {
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

}