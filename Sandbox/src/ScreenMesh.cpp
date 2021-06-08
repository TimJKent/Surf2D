#include "ScreenMesh.h"
#include "Group.h"

ScreenMesh::ScreenMesh()
	: m_Hidden(false),
		m_Selected(false),
		m_Group(-1),
		m_BorderColor(0.f,0.f,0.f,0.f),
		m_Name("Window"), 
		m_Resolution({ 1920.0f,1080.0f }),
		m_DisplayId("9999999")
{
	m_Mesh.reset(new MechEngine::Mesh());
	m_Mesh->AddVertex({-1.920f/2,	-1.080f/2,	0.f}, {0.f, 0.f}, MechEngine::Color(0.f,0.f,0.f,1.f));
	m_Mesh->AddVertex({ 1.920f/2,	-1.080f/2,	0.f}, {1.f, 0.f}, MechEngine::Color(0.f,0.f,0.f,1.f));
	m_Mesh->AddVertex({ 1.920f/2,	 1.080f/2, 0.f}, {1.f, 1.f}, MechEngine::Color(0.f,0.f,0.f,1.f));
	m_Mesh->AddVertex({-1.920f/2,	 1.080f/2, 0.f}, {0.f, 1.f}, MechEngine::Color(0.f,0.f,0.f,1.f));
	m_Mesh->AddIndex(0);
	m_Mesh->AddIndex(1);
	m_Mesh->AddIndex(2);
	m_Mesh->AddIndex(2);
	m_Mesh->AddIndex(3);
	m_Mesh->AddIndex(0);
}


void ScreenMesh::Draw3D() {
	glm::vec3 position = transform3d.GetPosition();
	
	if (!m_Hidden){
		MechEngine::Ref<MechEngine::Material> m = MechEngine::Renderer2D::GetMaterial(3);
		m->GetShader()->Bind();
		m->GetShader()->SetFloat4("u_Color",m_BorderColor.GetChannelRGBA());
			MechEngine::Transform t = transform3d;
			MechEngine::Transform t2 = transform3d;
			t.SetPosition(t.GetPosition() / 1000.f);
			t2.SetPosition(t2.GetPosition() / 1000.f);
			t.SetScale({ 1.02f, 1.02f, 1.0f });
			t.Translate(-m_Resolution.x*0.02f/2.f, -m_Resolution.y * 0.02f / 2.f,0.f);
			MechEngine::Renderer2D::DrawMesh(t, m_Mesh, 3);
			MechEngine::Renderer2D::DrawMesh(t2, m_Mesh, 2);
	}
}

void ScreenMesh::Draw2D() {
	if (!m_Hidden) {
		MechEngine::Ref<MechEngine::Material> m = MechEngine::Renderer2D::GetMaterial(3);
		m->GetShader()->Bind();
		m->GetShader()->SetFloat4("u_Color", m_BorderColor.GetChannelRGBA());
		MechEngine::Transform t = transform2d;
		t.SetScale({ 1.02f, 1.02f, 1.0f });
		t.Translate(-m_Resolution.x * 0.02f / 2.f, -m_Resolution.y * 0.02f / 2.f, 0.f);
		MechEngine::Renderer2D::DrawMesh(t, m_Mesh, 3);
		MechEngine::Renderer2D::DrawMeshWireFrame(transform2d, m_Mesh, 4);
	}
}


void ScreenMesh::SetHidden(bool hide) { 
	m_Hidden = hide;
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