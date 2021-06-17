#pragma once
#include "Components/Component.h"
#include "MechEngine/Core/Transform.h"

namespace MechEngine {
	class Object {
	public:
		Object();
			

		const const std::string& GetName() const;

		void SetName(const std::string& name);

		const const bool GetHidden() const;

		void SetHidden(bool hide);

		void OnUpdate();
		
		void DrawUI();

	public:
		Transform transform;
	protected:
		std::string m_Name;
		bool m_Hidden;
		std::vector<Component*> m_Components;
	};
}