#pragma once
#include "Event.h"


namespace SurfEngine {
	class SurfEngine_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}
		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		int m_Width, m_Height;
	};

	class SurfEngine_API WindowMoveEvent : public Event
	{
	public:
		WindowMoveEvent(unsigned int xPos, unsigned int yPos)
			: m_Xpos(xPos), m_Ypos(yPos) {}

		inline unsigned int GetPosX() const { return m_Xpos; }
		inline unsigned int GetPosY() const { return m_Ypos; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "WindowMoveEvent: " << m_Xpos << ", " << m_Ypos;
			return ss.str();
		}
		EVENT_CLASS_TYPE(WindowMoved)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		int m_Xpos, m_Ypos;
	};

	class SurfEngine_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}
		EVENT_CLASS_TYPE(WindowClose)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class SurfEngine_API AppTickEvent : public Event
	{
	public:
		AppTickEvent() {}
		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class SurfEngine_API AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() {}
		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class SurfEngine_API AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}
		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}