#include "ScreenMesh.h"
#include "Group.h"

ScreenMesh::ScreenMesh()
	:
		m_Selected(false),
		m_Group(-1),
		m_BorderColor(0.f,0.f,0.f,0.f),
		m_Resolution({ 1920.0f,1080.0f }),
		m_DisplayId("9999999")
{
	
}


void ScreenMesh::Draw3D() {
	glm::vec3 position = transform3d.GetPosition();
	
	if (!m_Enabled){
		MechEngine::Ref<MechEngine::Material> m = MechEngine::Renderer2D::GetMaterial(3);
		m->GetShader()->Bind();
		m->GetShader()->SetFloat4("u_Color",m_BorderColor.GetChannelRGBA());
			MechEngine::Transform t = transform3d;
			MechEngine::Transform t2 = transform3d;
			t.SetPosition(t.GetPosition() / 1000.f);
			t2.SetPosition(t2.GetPosition() / 1000.f);
			t.SetScale({ 1.02f, 1.02f, 1.0f });
			t.Translate(0.f,0.f,-0.01f);
		//	MechEngine::Renderer2D::DrawMesh(t, m_Mesh, 3);
			MechEngine::Renderer2D::DrawMesh(t2, m_Mesh, 2);
	}
}

void ScreenMesh::Draw2D() {

	if (!m_Enabled) {
		MechEngine::Ref<MechEngine::Material> m = MechEngine::Renderer2D::GetMaterial(3);
		m->GetShader()->Bind();
		m->GetShader()->SetFloat4("u_Color", m_BorderColor.GetChannelRGBA());
		MechEngine::Transform t = transform2d;
		t.SetPosition(t.GetPosition() / 1000.f);
		t.SetScale({ 1.0f, 1.0f, 1.0f });
		t.Translate(0.f, 0.f, -0.01f);
		//MechEngine::Renderer2D::DrawMesh(t, m_Mesh, 3);
		MechEngine::Renderer2D::DrawMeshWireFrame(t, m_Mesh, 4);
	}
}


bool ScreenMesh::IsInBounds(float x, float y) {
	if (x >= transform2d.GetPosition().x && y >= transform2d.GetPosition().y) {
		if (x <= transform2d.GetPosition().x + m_Resolution.x && y <= transform2d.GetPosition().y + m_Resolution.y) {
			return true;
		}
	}
	return false;
}

void ScreenMesh::SetSelected(std::vector<MechEngine::Ref<ScreenMesh>>& screens, int& currSelected, int setSelected) {
	screens[currSelected]->SetSelected(false);
	currSelected = setSelected;
	if (setSelected > -1)
		screens[currSelected]->SetSelected(true);
}

int ScreenMesh::GetGroupId() {
	return m_Group;
}

void ScreenMesh::SetGroup(int groupId) {
	m_Group = groupId;
}

void ScreenMesh::SetBorderColor(MechEngine::Color color) {
	m_BorderColor = color;
}