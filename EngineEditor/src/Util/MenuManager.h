#pragma once
#include "SurfEngine.h"
#include "ProjectManager.h"

#include "imgui/imgui.h"
#include "imgui/imconfig.h"
#include "imgui/imgui_internal.h"

using namespace SurfEngine;

class MenuManager {
public:
	static void Init();

	static void DrawNewProjectPopup();
	static void DrawNewScenePopup();
	static void DrawProjectPropertiesPopup();

	static void OpenScene(const std::string& filepath) {
		ProjectManager::OpenScene(filepath);
	}

	static void NewScene(const std::string& name) {
		ProjectManager::NewScene(name);
	}

	static void SaveScene() {
		ProjectManager::SaveCurrentScene();
	}

	static void BeginDialogue_OpenProject();


};


