#pragma once
#include <string>

namespace MechEngine {
	class Object;

	class Component {
	public:
		Component(Object* object);

		virtual void DrawUI() = 0;
		virtual void OnUpdate() = 0;
		
		virtual void Save() = 0;
		virtual void Load() = 0;

		virtual std::string UniqueId() = 0;

	public:
		bool IsEnabled = true;
	protected:
		Object* m_ParentObject;
	};
}