#pragma once
#include "ScreenMesh.h"
#include <algorithm> 
#include <string>
class ScreenList
{

public:

	void AddScreen() {
		MechEngine::Ref<ScreenMesh> s;
		s.reset(new ScreenMesh());
		s->SetTexture(MechEngine::Texture2D::Create("assets/textures/windows.png"));
		s->SetDisplayId("0");
		s->SetName("Window");
		m_CurrentSelectedScreen = 0;
		m_ScreenList.push_back(s);
	}

	bool IsEmpty() {
		return m_ScreenList.empty();
	}

	int Size() {
		return m_ScreenList.size();
	}

	int GetSelectedNumber() {
		return m_CurrentSelectedScreen;
	}
	
	void DeleteSelected() {
		m_ScreenList.erase(m_ScreenList.begin() + m_CurrentSelectedScreen);
		m_CurrentSelectedScreen = std::max(m_CurrentSelectedScreen-1,0);
	}

	void DeleteAll() {
		int size = m_ScreenList.size();
		for (int i = 0; i < size; i++) {
			m_ScreenList.erase(m_ScreenList.begin());
		}
		m_CurrentSelectedScreen = -1;
	}

	void SetSelectedPoint(glm::vec2 selectedPos, bool keepSelected) {
		if (IsEmpty()) {
			return;
		}
		MechEngine::Ref<ScreenMesh> s = m_ScreenList[0];
		glm::vec2 closestPoint = s->GetPoints()[0];
		float closestDistance = glm::distance(selectedPos, closestPoint);
		MechEngine::Ref<ScreenMesh> closestScreen = s;
		int closestSlot = 0;
		for (int i = 0; i < Size(); i++) {
			 s = m_ScreenList[i];
			 std::vector<glm::vec2> points = s->GetPoints();
			 for (int n = 0; n < 4; n++) {
				 if(!keepSelected)
					 s->SetSelectedVertex(n, false);
				 if (glm::distance(selectedPos, points[n]) < closestDistance) {
					 closestPoint = points[n];
					 closestDistance = glm::distance(selectedPos, points[n]);
					 closestScreen = s;
					 closestSlot = n;
				 }
			 }
		}
		if(closestDistance < 250.f)
			closestScreen->SetSelectedVertex(closestSlot, !closestScreen->GetPoint(closestSlot));
	}

	glm::vec3 FocusRotationOnSelected() {
		if (IsEmpty()) {
			return {0, 0, 0 };
		}

		float rotX = m_ScreenList[m_CurrentSelectedScreen]->transform3d.GetRotation().x;
		float rotY = m_ScreenList[m_CurrentSelectedScreen]->transform3d.GetRotation().y;
		return { rotY, rotX, 0 };
	}

	glm::vec3 FocusPositionOnSelected() {
		if (IsEmpty()) {
			return { 1000,-1000,1000 };
		}
		float posX = m_ScreenList[m_CurrentSelectedScreen]->transform3d.GetPosition().x + m_ScreenList[m_CurrentSelectedScreen]->GetResolution().x / 2;
		float posY = m_ScreenList[m_CurrentSelectedScreen]->transform3d.GetPosition().y + m_ScreenList[m_CurrentSelectedScreen]->GetResolution().y / 2;
		float posZ = m_ScreenList[m_CurrentSelectedScreen]->transform3d.GetPosition().z + 1500.f;
		return { posX, posY, posZ };
	}

	void DrawList(bool viewModeIs3d) {
	
		for (int i = Size() - 1; i >= 0; i--) {
			if (viewModeIs3d) {
				m_ScreenList[i]->Draw3D();
			}
			else {
				m_ScreenList[i]->Draw2D();
			}
		}
	}

	void SetSelectedScreen(int selected) {
		selected = std::max(selected, 0);
		selected = std::min(Size()-1, selected);
		m_ScreenList[m_CurrentSelectedScreen]->SetSelected(false);
		m_CurrentSelectedScreen = std::max(selected,0);
		m_ScreenList[m_CurrentSelectedScreen]->SetSelected(true);
	}

	void SetPrimary(int display) {
		for (int i = 0; i < m_ScreenList.size(); i++) {
			m_ScreenList[i]->SetPrimary(false);
		}
		m_ScreenList[display]->SetPrimary(true);
	}

	//Unsafe D:
	MechEngine::Ref<ScreenMesh>& Get(int i) {
		ME_ASSERT(i > 0 && i < Size(), "Index out of bound on screen list. index: {0}", i);
		return m_ScreenList[i];
	}

	MechEngine::Ref<ScreenMesh>& GetSelected() {
		ME_ASSERT(m_CurrentSelectedScreen > 0 && m_CurrentSelectedScreen < Size(), "Index out of bound on screen list. index: {0}", m_CurrentSelectedScreen);
		return m_ScreenList[m_CurrentSelectedScreen];
	}

	void MoveVerts(float dx, float dy) {
		for (int i = 0; i < Size(); i++) {
			m_ScreenList[i]->MoveSelectedVerts(dx, dy);
		}
	}

private:
	std::vector<MechEngine::Ref<ScreenMesh>> m_ScreenList;
	int m_CurrentSelectedScreen;
};

