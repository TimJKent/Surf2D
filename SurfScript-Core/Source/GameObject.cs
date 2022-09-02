using System;
using System.Runtime.CompilerServices;

namespace SurfEngine
{
	public class GameObject
	{
		protected GameObject() { ID = 0; }

		internal GameObject(ulong id)
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

	}

}