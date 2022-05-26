#include "SurfEngine.h"
#include "SurfEngine/Core/EntryPoint.h"

#include "Layers/EngineLayer.h"
#include "Layers/EditorLayer.h"


 namespace SurfEngine{

	class EditorApplication : public Application {
	public:
	
		EditorApplication() :
			m_EngineLayer(),
			m_EditorLayer(m_EngineLayer)
		{
			PushLayer(&m_EngineLayer);
			PushLayer(&m_EditorLayer);
		}

		~EditorApplication() {}
	private:
			EngineLayer m_EngineLayer;
			EditorLayer m_EditorLayer;
	};


	Application* CreateApplication() {
		return new EditorApplication();
	}
 }