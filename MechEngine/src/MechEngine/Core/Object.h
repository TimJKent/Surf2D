#pragma once

namespace MechEngine {
	class Component;

	class Object {
	public:
		Object();
		
		const const std::string& GetName() const;

		void SetName(const std::string& name);

		void OnUpdate();
		
		void DrawUI();

		template <typename T>
		int HasComponent() {
			for (int i = 0; i < m_Components.size(); i++) {
				if (typeid(T) == typeid(*m_Components[i])) {
					return i;
				}
			}
			return -1;
		}

		template <typename T>
		void AddComponent() {
			m_Components.push_back(std::make_shared<T>(this));
		}

		template <typename T>
		Ref<T> GetComponent(int slot) {
			return std::static_pointer_cast<T>(m_Components[slot]);
		}
	public:
		bool m_Enabled = true;
	protected:
		std::string m_Name;
		std::vector<Ref<Component>> m_Components;
	};
}