#include "Group.h"


void Group::AddScreen(MechEngine::Ref<ScreenMesh> screen) {
	m_ScreenList.push_back(screen);
	screen->SetBorderColor(m_Color);
}

ScreenMesh Group::GetScreen(int slot) {
	return *m_ScreenList[slot];
}

void Group::RemoveScreen(std::string name) {
	for (int i = 0; i < m_ScreenList.size(); i++) {
		if (name.compare(m_ScreenList[i]->GetName()) == 0) {
			m_ScreenList.erase(m_ScreenList.begin() + i);
			m_ScreenList[i]->SetBorderColor(MechEngine::Color(0.f,0.f,0.f,0.f));
		}
	}
}

int Group::Size() {
	return m_ScreenList.size();
}

const MechEngine::Color Group::GetColor() const {
	return m_Color;
}

void Group::SetColor(MechEngine::Color color) {
	m_Color = color;
	for (int i = 0; i < m_ScreenList.size(); i++) {
		m_ScreenList[i]->SetBorderColor(m_Color);
	}
}

void Group::SetName(const std::string& name) {
	m_Name = name;
}

std::string Group::GetName() {
	return m_Name;
}