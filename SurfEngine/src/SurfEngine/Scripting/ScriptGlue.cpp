#include "sepch.h"
#include "ScriptGlue.h"
#include "ScriptEngine.h"

#include "SurfEngine/Core/UUID.h"
#include "SurfEngine/Core/KeyCodes.h"
#include "SurfEngine/Core/Input.h"

#include "SurfEngine/Scenes/Scene.h"
#include "SurfEngine/Scenes/Components.h"
#include "SurfEngine/Scenes/Object.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

#include "box2d/b2_body.h"

namespace SurfEngine {

	static std::unordered_map<MonoType*, std::function<bool(Object)>> s_ObjectHasComponentFuncs;

#define SE_ADD_INTERNAL_CALL(Name) mono_add_internal_call("SurfEngine.InternalCalls::" #Name, Name)


	static void Debug_Log(MonoString* msg)
	{
		char* cStr = mono_string_to_utf8(msg);
		std::string str(cStr);
		mono_free(cStr);
		SE_TRACE(str);
	}

	static void Debug_Warn(MonoString* msg)
	{
		char* cStr = mono_string_to_utf8(msg);
		std::string str(cStr);
		mono_free(cStr);
		SE_WARN(str);
	}

	static void Debug_Error(MonoString* msg)
	{
		char* cStr = mono_string_to_utf8(msg);
		std::string str(cStr);
		mono_free(cStr);
		SE_ERROR(str);
	}

	static void Object_Instantiate(UUID* objectID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		
		Object o = scene->CreateObject();
		*objectID = o.GetUUID();
	}

	static void Object_Delete(UUID objectID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		scene->DeleteObject(object);
	}

	static bool Object_HasComponent(UUID objectID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		SE_CORE_ASSERT(s_objectHasComponentFuncs.find(managedType) != s_objectHasComponentFuncs.end());
		return s_ObjectHasComponentFuncs.at(managedType)(object);
	}

	static void AddComponentToObject(MonoType* managedType, Object o) {
		std::string_view typeName = mono_type_get_name(managedType);
		size_t pos = typeName.find_last_of('.');
		std::string_view compName = typeName.substr(pos + 1);
		
		if (compName._Equal("SpriteRendererComponent")) {
			o.AddComponent<SpriteRendererComponent>();
			return;
		}
		if (compName._Equal("AnimationComponent")) {
			o.AddComponent<AnimationComponent>();
			return;
		}
		if (compName._Equal("CameraComponent")) {
			o.AddComponent<CameraComponent>();
			return;
		}
		if (compName._Equal("ScriptComponent")) {
			o.AddComponent<ScriptComponent>();
			return;
		}
		if (compName._Equal("RigidbodyComponent")) {
			o.AddComponent<RigidbodyComponent>();
			return;
		}
		if (compName._Equal("BoxColliderComponent")) {
			o.AddComponent<BoxColliderComponent>();
			return;
		}
		if (compName._Equal("CircleColliderComponent")) {
			o.AddComponent<CircleColliderComponent>();
			return;
		}
		SE_WARN("Invalid Component Type");
	}


	static void Object_AddComponent(UUID objectID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);


		MonoType* managedType = mono_reflection_type_get_type(componentType);
		AddComponentToObject(managedType, object);
	}

	static void Object_RemoveComponent(UUID objectID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);


		MonoType* managedType = mono_reflection_type_get_type(componentType);

		std::string_view typeName = mono_type_get_name(managedType);
		size_t pos = typeName.find_last_of('.');
		std::string_view compName = typeName.substr(pos + 1);

		if (compName._Equal("SpriteRendererComponent")) {
			object.RemoveComponent<SpriteRendererComponent>();
			return;
		}
		if (compName._Equal("AnimationComponent")) {
			object.RemoveComponent<AnimationComponent>();
			return;
		}
		if (compName._Equal("CameraComponent")) {
			object.RemoveComponent<CameraComponent>();
			return;
		}
		if (compName._Equal("ScriptComponent")) {
			object.RemoveComponent<ScriptComponent>();
			return;
		}
		if (compName._Equal("RigidbodyComponent")) {
			object.RemoveComponent<RigidbodyComponent>();
			return;
		}
		if (compName._Equal("BoxColliderComponent")) {
			object.RemoveComponent<BoxColliderComponent>();
			return;
		}
		if (compName._Equal("CircleColliderComponent")) {
			object.RemoveComponent<CircleColliderComponent>();
			return;
		}
		SE_WARN("Invalid Component Type");
	}

	static MonoString* Object_GetName(UUID objectID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		auto& tc = object.GetComponent<TagComponent>();
		return  mono_string_new(mono_domain_get(), tc.Tag.c_str());
	}

	static void Object_SetName(UUID objectID, MonoString* name)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		char* cStr = mono_string_to_utf8(name);
		std::string str(cStr);
		mono_free(cStr);
		
		object.GetComponent<TagComponent>().Tag = str;
	}

	static bool TransformComponent_HasParent(UUID objectID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		return object.HasComponent<TransformComponent>();
	}

	
	static void TransformComponent_GetParent(UUID objectID, UUID* outParent)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		
		*outParent = object.GetComponent<TransformComponent>().parent->gameObject.GetUUID();
	}

	static void TransformComponent_SetParent(UUID objectID, UUID* parent)
	{
		if (objectID == *parent) { return; }

		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		Object objectParent = scene->GetObjectByUUID(*parent);
		SE_CORE_ASSERT(objectParent);
		
		if(objectParent)
			object.GetComponent<TransformComponent>().SetParent(&objectParent.GetComponent<TransformComponent>());

	}

	static void TransformComponent_GetTranslation(UUID objectID, glm::vec2* outTranslation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		*outTranslation = object.GetComponent<TransformComponent>().Translation;
	}

	static void TransformComponent_SetTranslation(UUID objectID, glm::vec2* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		object.GetComponent<TransformComponent>().Translation = *translation;
	}

	static void TransformComponent_GetRotation(UUID objectID, float* outRotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		*outRotation = object.GetComponent<TransformComponent>().Rotation;
	}

	static void TransformComponent_SetRotation(UUID objectID, float* rotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		object.GetComponent<TransformComponent>().Rotation = *rotation;
	}

	static void TransformComponent_GetScale(UUID objectID, glm::vec2* outScale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		*outScale = object.GetComponent<TransformComponent>().Scale;
	}

	static void TransformComponent_SetScale(UUID objectID, glm::vec2* scale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		object.GetComponent<TransformComponent>().Scale = *scale;
	}


	static void SpriteRendererComponent_GetColor(UUID objectID, glm::vec4* outColor)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		*outColor = object.GetComponent<SpriteRendererComponent>().Color;
	}

	static void SpriteRendererComponent_SetColor(UUID objectID, glm::vec4* color)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		object.GetComponent<SpriteRendererComponent>().Color = *color;
	}

	static void SpriteRendererComponent_GetLayer(UUID objectID, int* outLayer)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		*outLayer = object.GetComponent<SpriteRendererComponent>().Layer;
	}

	static void SpriteRendererComponent_SetLayer(UUID objectID, int* layer)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		object.GetComponent<SpriteRendererComponent>().Layer = *layer;
	}

	static void SpriteRendererComponent_GetFlipX(UUID objectID, bool* outFlipX)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		*outFlipX = object.GetComponent<SpriteRendererComponent>().flipX;
	}

	static void SpriteRendererComponent_SetFlipX(UUID objectID, bool* flipX)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		object.GetComponent<SpriteRendererComponent>().flipX = *flipX;
	}




	static void Rigidbody2DComponent_ApplyLinearImpulse(UUID objectID, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		auto& rb2d = object.GetComponent<RigidbodyComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}

	static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID objectID, glm::vec2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		SE_CORE_ASSERT(scene);
		Object object = scene->GetObjectByUUID(objectID);
		SE_CORE_ASSERT(object);

		auto& rb2d = object.GetComponent<RigidbodyComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}

	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	static bool Input_IsKeyUp(KeyCode keycode)
	{
		return !Input::IsKeyPressed(keycode);
	}

	template<typename... Component>
	static void RegisterComponent()
	{
		([]()
			{
				std::string_view typeName = typeid(Component).name();
				size_t pos = typeName.find_last_of(':');
				std::string_view structName = typeName.substr(pos + 1);
				std::string managedTypename = fmt::format("SurfEngine.{}", structName);

				MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
				if (!managedType)
				{
					SE_CORE_ERROR("Could not find component type {}", managedTypename);
					return;
				}
				s_ObjectHasComponentFuncs[managedType] = [](Object object) { return object.HasComponent<Component>(); };
			}(), ...);
	}

	
	template<typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}

	void ScriptGlue::RegisterComponents()
	{
		RegisterComponent(AllComponents{});
	}

	void ScriptGlue::RegisterFunctions()
	{
		//Debug
		{
			SE_ADD_INTERNAL_CALL(Debug_Log);
			SE_ADD_INTERNAL_CALL(Debug_Warn);
			SE_ADD_INTERNAL_CALL(Debug_Error);
		}
		
		//Object
		{
			SE_ADD_INTERNAL_CALL(Object_Instantiate);
			SE_ADD_INTERNAL_CALL(Object_Delete);
			SE_ADD_INTERNAL_CALL(Object_GetName);
			SE_ADD_INTERNAL_CALL(Object_SetName);
			SE_ADD_INTERNAL_CALL(Object_HasComponent);
			SE_ADD_INTERNAL_CALL(Object_AddComponent);
			SE_ADD_INTERNAL_CALL(Object_RemoveComponent);
		}

		//Input
		{
			SE_ADD_INTERNAL_CALL(Input_IsKeyDown);
			SE_ADD_INTERNAL_CALL(Input_IsKeyUp);
		}

		//Transform Componenet
		{
			SE_ADD_INTERNAL_CALL(TransformComponent_HasParent);
			SE_ADD_INTERNAL_CALL(TransformComponent_GetParent);
			SE_ADD_INTERNAL_CALL(TransformComponent_SetParent);
			SE_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
			SE_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
			SE_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
			SE_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
			SE_ADD_INTERNAL_CALL(TransformComponent_GetScale);
			SE_ADD_INTERNAL_CALL(TransformComponent_SetScale);
		}
		
		//SpriteRenderer Component
		{
			SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetColor);
			SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetColor);
			SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetLayer);
			SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetLayer);
			SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetFlipX);
			SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetFlipX);
		}

		SE_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
		SE_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);
		
		SE_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}

}
