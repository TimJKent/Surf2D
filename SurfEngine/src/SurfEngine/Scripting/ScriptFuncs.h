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
		if (std::strcmp(component_type_str, "CircleCollider") == 0) {
			return o.HasComponent<CircleColliderComponent>();
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
		tc.Translation.x = (float)x;
		tc.Translation.y = (float)-y;
		tc.Translation.z = (float)z;
	}

	void TranslateX(MonoString* msg, double x)
	{
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();
		tc.Translation.x += (float)x;
	}

	void TranslateY(MonoString* msg, double y)
	{
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();
		tc.Translation.y -= (float)y;
	}

	//Rotation
	double GetRotation(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();
		return (float)tc.Rotation.z;
	}

	void SetRotation(MonoString* msg, double new_rot) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();
		tc.Rotation.z = (float)new_rot;
	}


	void Rotate(MonoString* msg, double z)
	{
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();
		tc.Rotation.z += (float)z;
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
		tc.Scale.x = (float)x;
		tc.Scale.y = (float)y;
		tc.Scale.z = (float)z;
	}

	void ScaleX(MonoString* msg, double x)
	{
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();
		tc.Scale.x += (float)x;
	}

	void ScaleY(MonoString* msg, double y)
	{
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& tc = o.GetComponent<TransformComponent>();
		tc.Scale.y += (float)y;
	}

	MonoArray* GetColorImpl(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& src = o.GetComponent<SpriteRendererComponent>();

		MonoArray* arr = mono_array_new(mono_domain_get(), mono_get_double_class(), 4);
		mono_array_set(arr, double, 0, src.Color.r);
		mono_array_set(arr, double, 1, src.Color.g);
		mono_array_set(arr, double, 2, src.Color.b);
		mono_array_set(arr, double, 2, src.Color.a);

		return arr;
	}

	void SetColorImpl(MonoString* msg, double r, double g, double b, double a) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& src = o.GetComponent<SpriteRendererComponent>();
		src.Color.r = (float)r;
		src.Color.g = (float)g;
		src.Color.b = (float)b;
		src.Color.a = (float)a;
	}

	double GetLayerImpl(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& src = o.GetComponent<SpriteRendererComponent>();

		return src.Layer;
	}

	void SetLayerImpl(MonoString* msg, double layer) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& src = o.GetComponent<SpriteRendererComponent>();
		src.Layer = (unsigned int)layer;
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

	MonoArray* B2DGetOffsetImpl(MonoString* msg) {
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

	void B2DSetOffsetImpl(MonoString* msg, double x, double y) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& bc = o.GetComponent<BoxColliderComponent>();
		bc.Offset.x = (float)x;
		bc.Offset.y = (float)y;
	}

	MonoArray* B2DGetSizeImpl(MonoString* msg) {
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

	void B2DSetSizeImpl(MonoString* msg, double x, double y) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& bc = o.GetComponent<BoxColliderComponent>();
		bc.Size.x = (float)x;
		bc.Size.y = (float)y;
	}

	MonoArray* CC2DGetOffsetImpl(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& bc = o.GetComponent<CircleColliderComponent>();

		MonoArray* arr = mono_array_new(mono_domain_get(), mono_get_double_class(), 3);
		mono_array_set(arr, double, 0, bc.Offset.x);
		mono_array_set(arr, double, 1, bc.Offset.y);
		mono_array_set(arr, double, 2, 0);

		return arr;
	}

	void CC2DSetOffsetImpl(MonoString* msg, double x, double y) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& bc = o.GetComponent<CircleColliderComponent>();
		bc.Offset.x = (float)x;
		bc.Offset.y = (float)y;
	}

	double CC2DGetRadiusImpl(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& bc = o.GetComponent<CircleColliderComponent>();


		return (double)bc.Radius;
	}

	void CC2DSetRadiusImpl(MonoString* msg, double radius) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& bc = o.GetComponent<CircleColliderComponent>();
		bc.Radius = (float)radius;
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

		body->SetAngularVelocity((float)torque);
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

	double GetCameraSizeImpl(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& cc = o.GetComponent<CameraComponent>();
		return cc.Camera.GetOrthographicSize();
	}

	void SetCameraSizeImpl(MonoString* msg, double size) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& cc = o.GetComponent<CameraComponent>();
		cc.Camera.SetOrthographicSize((float)size);
	}

	unsigned int AnimationGetFrameImpl(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& ac = o.GetComponent<AnimationComponent>();
		return ac.currframe;
	}

	void AnimationSetFrameImpl(MonoString* msg, unsigned int frame) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& ac = o.GetComponent<AnimationComponent>();
		ac.currframe = frame;
	}

	void AnimationPlayImpl(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& ac = o.GetComponent<AnimationComponent>();
		ac.play = true;
	}

	void AnimationStopImpl(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& ac = o.GetComponent<AnimationComponent>();
		ac.play = false;
	}

	static void InitScriptFuncs() {

		//Debug
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

		mono_add_internal_call("SurfEngine.Transform::GetScaleImpl", &GetScale);
		mono_add_internal_call("SurfEngine.Transform::SetScaleImpl", &SetScale);
		mono_add_internal_call("SurfEngine.Transform::ScaleXImpl", &ScaleX);
		mono_add_internal_call("SurfEngine.Transform::ScaleYImpl", &ScaleY);

		mono_add_internal_call("SurfEngine.Transform::GetRotationImpl", &GetRotation);
		mono_add_internal_call("SurfEngine.Transform::SetRotationImpl", &SetRotation);
		mono_add_internal_call("SurfEngine.Transform::RotateImpl", &Rotate);

		//RigidBody
		mono_add_internal_call("SurfEngine.Rigidbody::AddForceImpl", &AddForceImpl);
		mono_add_internal_call("SurfEngine.Rigidbody::AddTorquImpl", &AddTorqueImpl);
		mono_add_internal_call("SurfEngine.Rigidbody::GetVelocityImpl", &GetVelocityImpl);
		mono_add_internal_call("SurfEngine.Rigidbody::GetTorqueImpl", &GetTorqueImpl);
		mono_add_internal_call("SurfEngine.Rigidbody::SetVelocityImpl", &SetVelocityImpl);
		mono_add_internal_call("SurfEngine.Rigidbody::SetTorqueImpl", &SetTorqueImpl);

		//Box Collider
		mono_add_internal_call("SurfEngine.BoxCollider::GetSizeImpl",   &B2DGetSizeImpl);
		mono_add_internal_call("SurfEngine.BoxCollider::SetSizeImpl",   &B2DSetSizeImpl);
		mono_add_internal_call("SurfEngine.BoxCollider::GetOffsetImpl", &B2DGetOffsetImpl);
		mono_add_internal_call("SurfEngine.BoxCollider::SetOffsetImpl", &B2DSetOffsetImpl);

		//Box Collider
		mono_add_internal_call("SurfEngine.CircleCollider::GetSizeImpl", &CC2DGetRadiusImpl);
		mono_add_internal_call("SurfEngine.CircleCollider::SetSizeImpl", &CC2DSetRadiusImpl);
		mono_add_internal_call("SurfEngine.CircleCollider::GetOffsetImpl", &CC2DGetOffsetImpl);
		mono_add_internal_call("SurfEngine.CircleCollider::SetOffsetImpl", &CC2DSetOffsetImpl);

		//Sprite Renderer
		mono_add_internal_call("SurfEngine.SpriteRenderer::GetLayerImpl", &GetLayerImpl);
		mono_add_internal_call("SurfEngine.SpriteRenderer::SetLayerImpl", &SetLayerImpl);
		mono_add_internal_call("SurfEngine.SpriteRenderer::GetColorImpl", &GetColorImpl);
		mono_add_internal_call("SurfEngine.SpriteRenderer::SetColorImpl", &SetColorImpl);
		mono_add_internal_call("SurfEngine.SpriteRenderer::FlipXImpl", &FlipX);

		//Camera
		mono_add_internal_call("SurfEngine.Camera::GetSizeImpl", &GetCameraSizeImpl);
		mono_add_internal_call("SurfEngine.Camera::SetSizeImpl", &SetCameraSizeImpl);

		//Animation
		mono_add_internal_call("SurfEngine.Animation::PlayImpl", &AnimationPlayImpl);
		mono_add_internal_call("SurfEngine.Animation::StopImpl", &AnimationStopImpl);
		mono_add_internal_call("SurfEngine.Animation::GetFrameImpl", &AnimationGetFrameImpl);
		mono_add_internal_call("SurfEngine.Animation::SetFrameImpl", &AnimationSetFrameImpl);

		//Input
		mono_add_internal_call("SurfEngine.Input::GetKeyDownImpl", &GetKeyDown);

	}
}