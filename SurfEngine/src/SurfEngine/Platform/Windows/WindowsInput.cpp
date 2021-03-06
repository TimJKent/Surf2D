#include "sepch.h"
#include "WindowsInput.h"

#include "SurfEngine/Core/Application.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

namespace SurfEngine {

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode) {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return (state == GLFW_PRESS || state == GLFW_REPEAT);
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button) {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	glm::vec2 WindowsInput::GetMousePositionImpl() {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return {x,y};
	}

	float WindowsInput::GetMouseXImpl() {
		return GetMousePositionImpl().x;
	}

	float WindowsInput::GetMouseYImpl() {
		return GetMousePositionImpl().y;
	}
}
