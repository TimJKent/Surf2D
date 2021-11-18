#pragma once

#include "Core.h"
#include "Window.h"
#include "SurfEngine/Core/LayerStack.h"

#include "SurfEngine/Events/Event.h"
#include "SurfEngine/Events/ApplicationEvent.h"
#include "SurfEngine/imgui/ImGuiLayer.h"

#include "SurfEngine/Core/Timestep.h"

#include "SurfEngine/Renderer/Shader.h"
#include "SurfEngine/Renderer/Buffer.h"
#include "SurfEngine/Renderer/VertexArray.h"

namespace SurfEngine{
	class SurfEngine_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		Window* m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Runing = true;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	//Defined In Client App
	Application* CreateApplication();
}


