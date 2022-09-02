using System;
using System.Runtime.CompilerServices;

namespace SurfEngine
{
	public static class InternalCalls
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static string Object_GetName(ulong objectID);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Object_SetName(ulong objectID, string name);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool Object_HasComponent(ulong objectID, Type componentType);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_GetTranslation(ulong entityID, out Vector2 translation);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_SetTranslation(ulong entityID, ref Vector2 translation);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_GetRotation(ulong entityID, out float rotation);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_SetRotation(ulong entityID, ref float rotation);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_GetScale(ulong entityID, out Vector2 scale);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_SetScale(ulong entityID, ref Vector2 scale);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Debug_Log(string msg);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Debug_Warn(string msg);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Debug_Error(string msg);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool Input_IsKeyDown(KeyCode keycode);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool Input_IsKeyUp(KeyCode keycode);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void SpriteRendererComponent_GetColor(ulong entityID, out Vector4 color);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void SpriteRendererComponent_SetColor(ulong entityID, ref Vector4 color);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void SpriteRendererComponent_GetLayer(ulong entityID, out int layer);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void SpriteRendererComponent_SetLayer(ulong entityID, ref int layer);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void SpriteRendererComponent_GetFlipX(ulong entityID, out bool flipX);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void SpriteRendererComponent_SetFlipX(ulong entityID, ref bool flipX);
	}
}