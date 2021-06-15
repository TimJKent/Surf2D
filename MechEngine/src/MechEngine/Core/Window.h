#pragma once
#include "mepch.h"
#include "MechEngine/Core/Core.h"
#include "MechEngine/Events/Event.h"

namespace MechEngine {
	struct WindowProps {
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		unsigned int posX;
		unsigned int posY;
		WindowProps(const std::string& title = "MechEngine",
			unsigned int width = 1280,
			unsigned int height = 720, 
			unsigned int posx= 50,
			unsigned int posy = 50)
			: Title(title), Width(width), Height(height), posX(posx), posY(posy) {}
	};

	class MECHENGINE_API Window{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}
		virtual void OnUpdate() = 0;
		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual unsigned int GetPosX() const = 0;
		virtual unsigned int GetPosY() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;
		
		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());

	};
}