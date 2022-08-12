#pragma once
#include "SurfEngine.h"
#include "ProjectManager.h"

namespace SurfEngine { namespace SceneManager {

	void CreateScene(const std::string& scene_name);

	void LoadScene(const std::string& filepath);
	void SaveCurrentScene();

	bool IsActiveScene();
	Ref<Scene>& GetActiveScene();
	void ClearActiveScene();


	bool IsSelectedObject();
	void SetSelectedObject(Ref<Object> object);
	Ref<Object> GetSelectedObject();
	void ClearSelectedObject();

}}