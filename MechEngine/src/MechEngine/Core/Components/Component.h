#pragma once


namespace MechEngine {
	class Object;

	class Component {
	public:
		Component(Object* object);

		virtual void DrawUI() = 0;
		virtual void OnUpdate() = 0;
		
	public:
		bool IsEnabled = true;
	protected:
		Object* m_ParentObject;
	};
}