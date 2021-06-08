#pragma once
#include "MechEngine/Core/Input.h"
#include "glm/glm.hpp"

namespace MechEngine {
	class WindowsInput : public Input
	{
	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;
		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
		virtual glm::vec2 GetMousePositionImpl() override;
	};
}
