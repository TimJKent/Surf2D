#pragma once
#include <string>

namespace MechEngine {
	class Object;

	class Component {
	public:
		Component(Object* object);

		virtual void DrawUI() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnDelete() = 0;

		virtual void Save() = 0;
		virtual void Load() = 0;

		virtual void SaveRefComponents() = 0;
		virtual void LoadRefComponents() = 0;

		virtual std::string UniqueId() = 0;

	public:
		bool IsEnabled = true;
		Object* m_ParentObject;
		int RequiredCounter = 0;
		int IdNumber;
	protected:
	};
}