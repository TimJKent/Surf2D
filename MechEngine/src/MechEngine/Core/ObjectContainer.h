#pragma once
#include "MechEngine/Core/Object.h"
#include <algorithm> 
#include <string>
namespace MechEngine {
	class ObjectContainer
	{

	public:

		void AddDefaultObject() {
			Ref<Object> object = std::make_shared<Object>();
			m_ObjectList.push_back(object);
			m_SelectedSlot++;
		}

		void Add(Ref<Object> o) {
			m_ObjectList.push_back(o);
			m_SelectedSlot++;

		}

		bool IsEmpty() {
			return m_ObjectList.empty();
		}

		int Size() {
			return m_ObjectList.size();
		}

		int GetSelectedNumber() {
			return m_SelectedSlot;
		}

		void Delete(int i) {
			if (m_SelectedSlot == i) { m_SelectedSlot--; }
			m_ObjectList.erase(m_ObjectList.begin() + i);
		
		}

		void DeleteSelected() {
			m_ObjectList.erase(m_ObjectList.begin() + m_SelectedSlot);
			m_SelectedSlot = m_SelectedSlot - 1;
		}

		void DeleteAll() {
			int size = m_ObjectList.size();
			for (int i = 0; i < size; i++) {
				m_ObjectList.erase(m_ObjectList.begin());
			}
			m_SelectedSlot = -1;
		}

		void UpdateList() {

			for (int i = Size() - 1; i >= 0; i--) {
					m_ObjectList[i]->OnUpdate();
			}
		}

		void SetSelectedObject(int selected) {
			selected = std::max(selected, -1);
			selected = std::min(Size() - 1, selected);
			m_SelectedSlot = std::max(selected, 0);
		}

		//Unsafe D:
		Ref<Object>& Get(int i) {
			ME_ASSERT(i > 0 && i < Size(), "Index out of bound on screen list. index: {0}", i);
			return m_ObjectList[i];
		}

		Ref<Object>& GetSelected() {
			ME_ASSERT(m_CurrentSelectedScreen > 0 && m_CurrentSelectedScreen < Size(), "Index out of bound on screen list. index: {0}", m_CurrentSelectedScreen);
			return m_ObjectList[m_SelectedSlot];
		}

		void Save() {
			MechEngine::Serialization::OpenFileForReadAndWrite("C:\\Users\\timbe\\Desktop\\testFolder", "test");
			MechEngine::Serialization::SERIAL_WRITE(Size());
			for (int i = 0; i < Size(); i++) {
				m_ObjectList[i]->Save();
			}
			MechEngine::Serialization::CloseFile();
		}

	private:
		std::vector<Ref<Object>> m_ObjectList;
		int m_SelectedSlot =-1;
	};
}


