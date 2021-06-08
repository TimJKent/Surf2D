#pragma once
#include <vector>
#include "MechEngine.h"
#include "ScreenMesh.h"
#include "glm/glm.hpp"


class Group {
public:
	Group(int id) : m_Color(1.f,1.f,1.f), m_Name(){
		m_id = id;
	}

	int GetId() {
		return m_id;
	}
	
	void AddScreen(MechEngine::Ref<ScreenMesh> screen);

	ScreenMesh GetScreen(int slot);

	void RemoveScreen(std::string name);

	int Size();

	const MechEngine::Color GetColor() const;

	void SetColor(MechEngine::Color color);

	void SetName(const std::string& name);

	std::string GetName();

private:
	int m_id;
	MechEngine::Color m_Color;
	std::vector<MechEngine::Ref<ScreenMesh>> m_ScreenList;
	std::string m_Name;
};