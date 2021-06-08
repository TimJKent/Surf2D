#pragma once
#include "MechEngine.h"
#include "glm/gtc/matrix_transform.hpp"


class Group;

class ScreenMesh {
public:
	ScreenMesh();
	~ScreenMesh() {}

	void Draw2D();
	void Draw3D();

	inline bool IsHidden() const { return m_Hidden; }
	void SetHidden(bool hide);

	inline bool IsPrimary() const { return m_Primary;}
	void SetPrimary(bool primary) { m_Primary = primary; }

	void SetSelected(const bool selected) {
		m_Selected = selected;
	}

	int GetGroupId();

	void SetGroup(int groupId);

	inline std::string GetName()  { return m_Name; }
	void SetName(const std::string& name) { m_Name = name; }

	inline glm::vec2 GetResolution() const { return { m_Resolution.x, m_Resolution.y }; }
	void SetResolution(glm::vec2 resolution) { m_Resolution = { resolution.x,resolution.y}; }

	inline std::string GetDisplayId() const { return m_DisplayId; }
	void SetDisplayId(const std::string& id) { m_DisplayId = id; }

	void SetTexture(const MechEngine::Ref<MechEngine::Texture2D>& texture)  {
		m_Texture = texture;
	}

	bool IsInBounds(float x, float y);

	void SetSelectedVertex(int slot, bool selected) {
		MechEngine::Color c = selected ? MechEngine::Color(1.f, .5f, 0.f, 1.0f) : MechEngine::Color(0.f,0.f,0.f,1.f);
		m_Mesh->SetColor(slot, c);
	}

	void MoveSelectedVerts(float dx, float dy) {
		for (int i = 0; i < m_Mesh->Size(); i++) {
			if (m_Mesh->GetColor(i) == MechEngine::Color(1.f, .5f, 0.f, 1.0f)) {
				m_Mesh->SetPosition(i,{dx - transform2d.GetPosition().x,dy + transform2d.GetPosition().y,0.f });
			}
		}
	}

	std::vector<glm::vec2> GetPoints(){
		std::vector<glm::vec2> points;
		for (int i = 0; i < m_Mesh->Size(); i++) {
			glm::vec2 pos = m_Mesh->GetVertexPosition(i);
			pos.x += transform2d.GetPosition().x;
			pos.y -= transform2d.GetPosition().y;
			points.push_back(pos);
		}
		return points;
	}

	bool GetPoint(int slot) {
		MechEngine::Color pointColor = m_Mesh->GetColor(slot);
		MechEngine::Color selectedColor = {1.f,0.5f,0.f,1.f};
		return pointColor == selectedColor;
	}

	void SetBorderColor(MechEngine::Color);

public:
	static void SetSelected(std::vector<MechEngine::Ref<ScreenMesh>>& screens, int& currSelected, int setSelected);
	static void Sort(std::vector<MechEngine::Ref<ScreenMesh>>& screens) {
		std::sort(screens.begin(), screens.end(), [](const MechEngine::Ref<ScreenMesh>& s1, const MechEngine::Ref<ScreenMesh>& s2) {
			glm::vec2 s1pos = s1->transform2d.GetPosition();
			glm::vec2 s2pos = s2->transform2d.GetPosition();
			if (s1pos.y < s2pos.y) {
				return true;
			}
			if (s1pos.y == s2pos.y) {
				if (s1pos.x < s2pos.x) {
					return true;
				}
			}
			return false;
			});
	}
	static void PrintList(std::vector<MechEngine::Ref<ScreenMesh>>& screens) {
		ME_WARN("-------------------------------------");
		for (int i = 0; i < screens.size(); i++) {
			ME_INFO("{0}",screens[i]->GetName());
		}
		ME_WARN("-------------------------------------");
	}
	static int GetRows(std::vector<MechEngine::Ref<ScreenMesh>>& screens) {
		std::vector<int> rows;
		for (int i = 0; i < screens.size(); i++) {
			if (std::find(rows.begin(), rows.end(), screens[i]->transform2d.GetPosition().y) == rows.end()) {
				rows.push_back(screens[i]->transform2d.GetPosition().y);
			}
		}
		return rows.size();
	}
	static int GetColumns(std::vector<MechEngine::Ref<ScreenMesh>>& screens) {
		std::vector<int> columns;
		for (int i = 0; i < screens.size(); i++) {
			if (std::find(columns.begin(), columns.end(), screens[i]->transform2d.GetPosition().x) == columns.end()) {
				columns.push_back(screens[i]->transform2d.GetPosition().x);
			}	
		}
		return columns.size();
	}

	MechEngine::Transform transform2d;
	MechEngine::Transform transform3d;

private:
	bool m_Primary = false;
	bool m_Hidden = false;
	bool m_Selected;
	int m_Group = -1;
	MechEngine::Color m_BorderColor;
	MechEngine::Ref<MechEngine::Mesh> m_Mesh;
	std::string m_Name;
	glm::vec2 m_Resolution;
	std::string m_DisplayId;
	MechEngine::Ref<MechEngine::Texture2D> m_Texture;
};