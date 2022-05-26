#include "SurfEngine.h"
#include "SurfEngine/Core/EntryPoint.h"

#include "Layers/EngineLayer.h"
#include "Layers/EditorLayer.h"


 namespace SurfEngine{
	class EditorApplication : public Application {
	public:
		EditorApplication(){
			EngineLayer* runtime = new EngineLayer();
			EditorLayer* editor = new EditorLayer(*runtime);

			PushLayer(runtime);
			PushLayer(editor);
		}
		~EditorApplication() {}
	};

	Application* CreateApplication() {
		return new EditorApplication();
	}
 
 }