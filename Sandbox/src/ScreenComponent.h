#pragma once
#include "MechEngine.h"
#include "glm/glm.hpp"


class ScreenComponent : public MechEngine::Component{
public:
	ScreenComponent(MechEngine::Object* object);
	void OnUpdate() override;
	void DrawUI() override;
	
private:
	glm::vec2 m_Resolution;
};