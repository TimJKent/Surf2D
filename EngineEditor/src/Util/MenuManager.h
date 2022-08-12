#pragma once
#include "SurfEngine.h"
#include "ProjectManager.h"
#include "SceneManager.h"

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

	static void LoadScene(const std::string& filepath) {
		SceneManager::LoadScene(filepath);
	}

	static void NewScene(const std::string& name) {
		SceneManager::CreateScene(name);
	}

	static void SaveScene() {
		SceneManager::SaveCurrentScene();
	}

	static void BeginDialogue_OpenProject();


};


