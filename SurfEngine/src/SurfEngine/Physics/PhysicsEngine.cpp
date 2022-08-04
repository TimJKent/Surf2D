#include "sepch.h"
#include "PhysicsEngine.h"

#include "SurfEngine/Scenes/Components.h"
#include "SurfEngine/Scenes/Scene.h"
#include "SurfEngine/Core/Timestep.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

namespace SurfEngine {

	static b2BodyType Rigidbody2DTypeToBox2DBody(RigidbodyComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case RigidbodyComponent::BodyType::Static:    return b2_staticBody;
		case RigidbodyComponent::BodyType::Dynamic:   return b2_dynamicBody;
		case RigidbodyComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		SE_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

	PhysicsData PhysicsEngine::s_Data = {};
	b2World* PhysicsEngine::m_PhysicsWorld = nullptr;

	void PhysicsEngine::OnPhysics2DStart(Scene* scene) {
		m_PhysicsWorld = new b2World({ s_Data.gravity_scale.x, s_Data.gravity_scale.y });

		auto view = scene->GetRegistry()->view<RigidbodyComponent>();
		for (auto o : view)
		{
			Object object = { o, scene };
			auto& transform = object.GetComponent<TransformComponent>();
			auto& rb2d = object.GetComponent<RigidbodyComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Translation.x, -transform.Translation.y);
			bodyDef.angle = -glm::radians(transform.Rotation.z);

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);

			rb2d.RuntimeBody = body;

			if (object.HasComponent<BoxColliderComponent>())
			{
				auto& bc2d = object.GetComponent<BoxColliderComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x / 2 * transform.Scale.x, bc2d.Size.y / 2 * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.physics_material.Density;
				fixtureDef.friction = bc2d.physics_material.Friction;
				fixtureDef.restitution = bc2d.physics_material.Restitution;
				fixtureDef.restitutionThreshold = bc2d.physics_material.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
			if (object.HasComponent<CircleColliderComponent>())
			{
				auto& cc2d = object.GetComponent<CircleColliderComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x,cc2d.Offset.y);
				circleShape.m_radius = cc2d.Radius * transform.Scale.x;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.physics_material.Density;
				fixtureDef.friction = cc2d.physics_material.Friction;
				fixtureDef.restitution = cc2d.physics_material.Restitution;
				fixtureDef.restitutionThreshold = cc2d.physics_material.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void PhysicsEngine::OnPhysics2DStop() {
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void PhysicsEngine::OnPhysics2DUpdate(Timestep ts, Scene* scene) {
		m_PhysicsWorld->Step(ts, s_Data.velocity_iterations, s_Data.position_iterations);

		// Retrieve transform from Box2D
		auto view = scene->GetRegistry()->view<RigidbodyComponent>();
		for (auto o : view)
		{
			Object object = { o, scene };
			auto& transform = object.GetComponent<TransformComponent>();
			auto& rb2d = object.GetComponent<RigidbodyComponent>();

			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			const auto& position = body->GetPosition();
			transform.Translation.x = position.x;
			transform.Translation.y = -position.y;
			transform.Rotation.z = -glm::degrees(body->GetAngle());
		}
	}
}