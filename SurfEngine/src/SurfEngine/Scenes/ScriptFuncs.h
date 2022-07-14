#pragma once
#include "SurfEngine.h"
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "box2d/b2_body.h"

namespace SurfEngine {
	static Scene* current_scene;

	MonoString* GetName(MonoString* obj_uuid)
	{
		char* uuid_str = mono_string_to_utf8(obj_uuid);
		uint64_t uuid = std::stoull(uuid_str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TagComponent>();
		return mono_string_new(mono_domain_get(), tc.Tag.c_str());
	}

	bool HasComponent(MonoString* obj_uuid, MonoString* component_type)
	{
		char* uuid_str = mono_string_to_utf8(obj_uuid);
		uint64_t uuid = std::stoull(uuid_str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));

		char* component_type_str = mono_string_to_utf8(component_type);
		if (std::strcmp(component_type_str, "Transform") == 0) {
			return o.HasComponent<TransformComponent>();
		}
		if (std::strcmp(component_type_str, "SpriteRenderer") == 0) {
			return o.HasComponent<SpriteRendererComponent>();
		}
		if (std::strcmp(component_type_str, "Camera") == 0) {
			return o.HasComponent<CameraComponent>();
		}
		if (std::strcmp(component_type_str, "Animation") == 0) {
			return o.HasComponent<AnimationComponent>();
		}
		if (std::strcmp(component_type_str, "Rigidbody") == 0) {
			return o.HasComponent<RigidbodyComponent>();
		}
		if (std::strcmp(component_type_str, "BoxCollider") == 0) {
			return o.HasComponent<BoxColliderComponent>();
		}
		return false;
	}

	MonoString* GetComponent(MonoString* obj_uuid, MonoString* component_type)
	{
		std::string out_uuid = "";
		char* uuid_str = mono_string_to_utf8(obj_uuid);
		uint64_t uuid = std::stoull(uuid_str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));

		if (HasComponent(obj_uuid, component_type)) {
			return obj_uuid;
		}

		return mono_string_new(mono_domain_get(), out_uuid.c_str());
	}

	bool GetKeyDown(int keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	//POSITION
	MonoArray* GetPosition(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();

		MonoArray* arr = mono_array_new(mono_domain_get(), mono_get_double_class(), 3);
		mono_array_set(arr, double, 0, tc.Translation.x);
		mono_array_set(arr, double, 1, -tc.Translation.y);
		mono_array_set(arr, double, 2, tc.Translation.z);

		return arr;
	}

	void SetPosition(MonoString* msg, double x, double y, double z) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();
		tc.Translation.x = x;
		tc.Translation.y = -y;
		tc.Translation.z = z;
	}

	void TranslateX(MonoString* msg, double x)
	{
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();
		tc.Translation.x += x;
	}

	void TranslateY(MonoString* msg, double y)
	{
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();
		tc.Translation.y -= y;
	}

	//Rotation
	double GetRotation(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();
		return tc.Rotation.z;
	}

	void SetRotation(MonoString* msg, double new_rot) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();
		tc.Rotation.z = new_rot;
	}


	void Rotate(MonoString* msg, double z)
	{
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();
		tc.Rotation.z += z;
	}

	//Scale
	MonoArray* GetScale(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();

		MonoArray* arr = mono_array_new(mono_domain_get(), mono_get_double_class(), 3);
		mono_array_set(arr, double, 0, tc.Scale.x);
		mono_array_set(arr, double, 1, tc.Scale.y);
		mono_array_set(arr, double, 2, tc.Scale.z);

		return arr;
	}

	void SetScale(MonoString* msg, double x, double y, double z) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();
		tc.Scale.x = x;
		tc.Scale.y = y;
		tc.Scale.z = z;
	}

	void ScaleX(MonoString* msg, double x)
	{
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();
		tc.Scale.x += x;
	}

	void ScaleY(MonoString* msg, double y)
	{
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();
		tc.Scale.y += y;
	}

	//Sprite Renderer
	void FlipX(MonoString* msg, bool flipX)
	{
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& sr = o.GetComponent<SpriteRendererComponent>();
		sr.flipX = flipX;
	}

	MonoString* GetGameObjectByName(MonoString* name)
	{
		char* name_str = mono_string_to_utf8(name);
		Object o = current_scene->GetObjectByName(name_str);
		return mono_string_new(mono_domain_get(), o.GetComponent<TagComponent>().uuid.ToString().c_str());
	}

	MonoArray* GetOffsetImpl(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& bc = o.GetComponent<BoxColliderComponent>();

		MonoArray* arr = mono_array_new(mono_domain_get(), mono_get_double_class(), 3);
		mono_array_set(arr, double, 0, bc.Offset.x);
		mono_array_set(arr, double, 1, bc.Offset.y);
		mono_array_set(arr, double, 2, 0);

		return arr;
	}

	void SetOffsetImpl(MonoString* msg, double x, double y) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& bc = o.GetComponent<BoxColliderComponent>();
		bc.Offset.x = x;
		bc.Offset.y = y;
	}

	MonoArray* GetSizeImpl(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& bc = o.GetComponent<BoxColliderComponent>();

		MonoArray* arr = mono_array_new(mono_domain_get(), mono_get_double_class(), 3);
		mono_array_set(arr, double, 0, bc.Size.x);
		mono_array_set(arr, double, 1, bc.Size.y);
		mono_array_set(arr, double, 2, 0);

		return arr;
	}

	void SetSizeImpl(MonoString* msg, double x, double y) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& bc = o.GetComponent<BoxColliderComponent>();
		bc.Size.x = x;
		bc.Size.y = y;
	}

	//DEBUG
	void Log(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		SE_INFO(str);
		mono_free(str);
	}

	void Warn(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		SE_WARN(str);
		mono_free(str);
	}

	void Error(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		SE_ERROR(str);
		mono_free(str);
	}

	void AddForceImpl(MonoString* msg, double x, double y, double z, int type) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& rbc = o.GetComponent<RigidbodyComponent>();
		
		b2Body* body = static_cast<b2Body*>(rbc.RuntimeBody);

		if (type == 2) { body->ApplyLinearImpulseToCenter({ (float)x,(float)y }, true); return; }
		body->ApplyForceToCenter({ (float)x,(float)y }, true);
	}

	void SetVelocityImpl(MonoString* msg, double x, double y, double z) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& rbc = o.GetComponent<RigidbodyComponent>();

		b2Body* body = static_cast<b2Body*>(rbc.RuntimeBody);

		body->SetLinearVelocity({ (float)x,(float)y });
	}

	void AddTorqueImpl(MonoString* msg, double torque, int type) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& rbc = o.GetComponent<RigidbodyComponent>();

		b2Body* body = static_cast<b2Body*>(rbc.RuntimeBody);

		if (type == 2) { body->ApplyTorque({ (float)torque }, true); return; }
		body->ApplyAngularImpulse({ (float)torque}, true);
	}

	void SetTorqueImpl(MonoString* msg, double torque) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& rbc = o.GetComponent<RigidbodyComponent>();

		b2Body* body = static_cast<b2Body*>(rbc.RuntimeBody);

		body->SetAngularVelocity(torque);
	}

	MonoArray* GetVelocityImpl(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& rbc = o.GetComponent<RigidbodyComponent>();
		b2Body* body = static_cast<b2Body*>(rbc.RuntimeBody);

		MonoArray* arr = mono_array_new(mono_domain_get(), mono_get_double_class(), 3);
		mono_array_set(arr, double, 0,  body->GetLinearVelocity().x);
		mono_array_set(arr, double, 1, body->GetLinearVelocity().y);
		mono_array_set(arr, double, 2, 0);

		return arr;
	}

	double GetTorqueImpl(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& rbc = o.GetComponent<RigidbodyComponent>();
		b2Body* body = static_cast<b2Body*>(rbc.RuntimeBody);

		return  body->GetAngularVelocity();
	}

	static void InitScriptFuncs() {

		mono_add_internal_call("SurfEngine.Debug::LogImpl", &Log);
		mono_add_internal_call("SurfEngine.Debug::WarnImpl", &Warn);
		mono_add_internal_call("SurfEngine.Debug::ErrorImpl", &Error);

		//GameObject
		mono_add_internal_call("SurfEngine.GameObject::GetNameImpl", &GetName);
		mono_add_internal_call("SurfEngine.GameObject::HasComponentImpl", &HasComponent);
		mono_add_internal_call("SurfEngine.GameObject::GetComponentImpl", &GetComponent);
		mono_add_internal_call("SurfEngine.GameObject::GetGameObjectByNameImpl", &GetGameObjectByName);

		//Transform
		mono_add_internal_call("SurfEngine.Transform::GetPositionImpl", &GetPosition);
		mono_add_internal_call("SurfEngine.Transform::SetPositionImpl", &SetPosition);
		mono_add_internal_call("SurfEngine.Transform::TranslateXImpl", &TranslateX);
		mono_add_internal_call("SurfEngine.Transform::TranslateYImpl", &TranslateY);

		mono_add_internal_call("SurfEngine.SpriteRenderer::FlipXImpl", &FlipX);

		mono_add_internal_call("SurfEngine.Transform::GetScaleImpl", &GetScale);
		mono_add_internal_call("SurfEngine.Transform::SetScaleImpl", &SetScale);
		mono_add_internal_call("SurfEngine.Transform::ScaleXImpl", &ScaleX);
		mono_add_internal_call("SurfEngine.Transform::ScaleYImpl", &ScaleY);

		mono_add_internal_call("SurfEngine.Transform::GetRotationImpl", &GetRotation);
		mono_add_internal_call("SurfEngine.Transform::SetRotationImpl", &SetRotation);
		mono_add_internal_call("SurfEngine.Transform::RotateImpl", &Rotate);

		//Physics
		//RigidBody
		mono_add_internal_call("SurfEngine.Rigidbody::AddForceImpl", &AddForceImpl);
		mono_add_internal_call("SurfEngine.Rigidbody::AddTorquImpl", &AddTorqueImpl);
		mono_add_internal_call("SurfEngine.Rigidbody::GetVelocityImpl", &GetVelocityImpl);
		mono_add_internal_call("SurfEngine.Rigidbody::GetTorqueImpl", &GetTorqueImpl);
		mono_add_internal_call("SurfEngine.Rigidbody::SetVelocityImpl", &SetVelocityImpl);
		mono_add_internal_call("SurfEngine.Rigidbody::SetTorqueImpl", &SetTorqueImpl);

		//Box Collider
		mono_add_internal_call("SurfEngine.BoxCollider::GetSizeImpl", &GetSizeImpl);
		mono_add_internal_call("SurfEngine.BoxCollider::SetSizeImpl", &SetSizeImpl);
		mono_add_internal_call("SurfEngine.BoxCollider::GetOffsetImpl", &GetOffsetImpl);
		mono_add_internal_call("SurfEngine.BoxCollider::SetOffsetImpl", &SetOffsetImpl);

		//Input
		mono_add_internal_call("SurfEngine.Input::GetKeyDownImpl", &GetKeyDown);

	}
}