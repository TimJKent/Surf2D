#pragma once
#include "SurfEngine.h"
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "box2d/b2_body.h"

namespace SurfEngine {
	static Scene* current_scene;

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

	void AnimationSetFpsImpl(MonoString* msg, int fps) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& ac = o.GetComponent<AnimationComponent>();
		ac.fps = fps;
	}

	int AnimationGetFpsImpl(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		uint64_t uuid = std::stoull(str);
		Object o = current_scene->GetObjectByUUID(UUID(uuid));
		auto& ac = o.GetComponent<AnimationComponent>();
		return ac.fps;
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
}