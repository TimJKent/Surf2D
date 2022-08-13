#pragma once
#include "SurfEngine.h"

namespace SurfEngine { namespace SceneManager {

	void CreateScene(const std::string& scene_name);

	void LoadScene(const std::string& filepath);
	void SaveCurrentScene();

	bool IsActiveScene();
	Ref<Scene>& GetActiveScene();
	void ClearActiveScene();


	bool IsSelectedObject();
	void PushObjectToSelection(Ref<Object> object);
	void RemoveObjectFromSelection(Ref<Object> object);
	std::size_t GetSelectedObjectCount();
	std::vector<Ref<Object>>& GetSelectedObject();
	void ClearSelectedObject();
	bool IsObjectSelected(Ref<Object> object);

	template<typename T, typename... Args>
	void AddComponentToSelected(Args&&... args) {
		for (int i = 0; i < GetSelectedObjectCount(); i++) {
			if (!GetSelectedObject()[i]->HasComponent<T>()) {
				GetSelectedObject()[i]->AddComponent<T>(std::forward<Args>(args)...);
			}
		}
	}

	template<typename T>
	bool HasComponenetForAllSelected() {
		for (int i = 0; i < GetSelectedObjectCount(); i++) {
			if (!GetSelectedObject()[i]->HasComponent<T>()) {
				return false;
			}
		}
		return true;
	}

	void PushAllObjectsToSelection();
	void DuplicateSelected();
	void DeleteSelected();

	const std::string& GetCurrentScenePath();
}}