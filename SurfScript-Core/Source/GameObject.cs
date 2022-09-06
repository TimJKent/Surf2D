using System;
using System.Runtime.CompilerServices;

namespace SurfEngine
{
	public class GameObject
	{
		public static GameObject Instantiate() {
			ulong uuid;
			InternalCalls.Object_Instantiate(out uuid);
			GameObject gameObject = new GameObject(uuid);
			return gameObject;
		}

		public void Delete() {
			ulong uuid = this.ID;
			InternalCalls.Object_Delete(uuid);
		}

		protected GameObject() { ID = 0; }

		public GameObject(ulong id)
		{
			ID = id;
		}

		public readonly ulong ID;

		public string GetName()
		{
			return InternalCalls.Object_GetName(ID);		
		}

		public void SetName(string name)
		{
			InternalCalls.Object_SetName(ID,  name);
		}

		public bool HasComponent<T>() where T : Component, new()
		{
			Type componentType = typeof(T);
			return InternalCalls.Object_HasComponent(ID, componentType);
		}

		public T GetComponent<T>() where T : Component, new()
		{
			if (!HasComponent<T>())
				return null;

			T component = new T() { GameObject = this };
			return component;
		}

		public T AddComponent<T>() where T : Component, new()
		{
			Type componentType = typeof(T);
			InternalCalls.Object_AddComponent(ID, componentType);
			T component = new T() { GameObject = this };
			return component;
		}

		public void RemoveComponent<T>() where T : Component, new()
		{	
			Type componentType = typeof(T);
			InternalCalls.Object_RemoveComponent(ID, componentType);
		}

	}

}