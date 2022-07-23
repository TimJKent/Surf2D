#include "sepch.h"
#include "Scene.h"

#include "Components.h"
#include "SurfEngine/Core/KeyCodes.h"
#include "SurfEngine/Renderer/Renderer2D.h"
#include "SurfEngine/Scenes/Object.h"

#include <filesystem>

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"

#include "SurfEngine/Scripting/ScriptEngine.h"

#include <glm/glm.hpp>

// Box2D
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

	Scene::Scene(){
		m_Registry = entt::registry();
	}

	Scene::~Scene() {
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::OnUpdateRuntime(Timestep ts) {	
		//Create Scene Cameras from Camera Components
		m_Registry.view<CameraComponent>().each([=](auto object, CameraComponent& cc) {
			m_sceneCamera = std::make_shared<SceneCamera>(cc.Camera);
			});

		// Physics
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

			// Retrieve transform from Box2D
			auto view = m_Registry.view<RigidbodyComponent>();
			for (auto o : view)
			{
				Object object = {o, this };
				auto& transform = object.GetComponent<TransformComponent>();
				auto& rb2d = object.GetComponent<RigidbodyComponent>();

				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				const auto& position = body->GetPosition();
				transform.Translation.x = position.x;
				transform.Translation.y = -position.y;
				transform.Rotation.z = -body->GetAngle();
			}
		}
		

		m_Registry.view<ScriptComponent>().each([=](auto object, ScriptComponent& cc) {
			ScriptEngine::SetCurrentScene(this);
			MonoClassField* field;
			void* iter = NULL;
			//while ((field = mono_class_get_fields(cc.monoclass.monoclass, &iter))) {
			//	std::string name = mono_field_get_name(field);
			//	for (int i = 0; i < cc.variables.size(); i++) {
			//		if (cc.variables[i].name._Equal(name)) {
			//			if (cc.variables[i].type._Equal("double")) {
			//				double d = std::stod(cc.variables[i].user_value);
			//				mono_field_get_value(cc.script_class_instance, field, &d);
			//			}
			//			if (cc.variables[i].type._Equal("int")) {
			//				int i = std::stoi(cc.variables[i].user_value);
			//				mono_field_get_value(cc.script_class_instance, field, &i);
			//			}
			//			if (cc.variables[i].type._Equal("string")) {
			//				mono_field_get_value(cc.script_class_instance, field, &cc.variables[i].user_value);
			//			}
			//			if (cc.variables[i].type._Equal("bool")) {
			//				bool b = cc.variables[i].user_value._Equal("true");
			//				mono_field_get_value(cc.script_class_instance, field, &b);
			//			}
			//			break;
			//		}
			//	}
			//}
			delete iter;
			cc.monoclass.InvokeMethod(cc.script_class_instance, cc.monoclass.GetMethod("OnUpdate", 0));
		});

		if (m_sceneCamera) {
			Renderer2D::BeginScene(m_sceneCamera.get());
			
			//Do Animations
			auto animgroup = m_Registry.group<AnimationComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : animgroup) {
				auto [anim, sprite] = animgroup.get<AnimationComponent, SpriteRendererComponent>(entity);
				if (anim.play) {
					anim.timer += ts;
				}
				if (anim.timer > (1.0f/anim.fps)) {
					anim.timer = 0.0f;
					anim.currframe++;
					if (anim.currframe > anim.frames) {
						if (!anim.loop) {
							anim.play = false;
						}
						anim.currframe = 1;
					}
				}

				sprite.currFrame = anim.currframe;
				sprite.totalFrames = anim.frames;

			}

			//Draw Sprites
			auto group = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
			group.sort<SpriteRendererComponent>([](const SpriteRendererComponent& lhs, const SpriteRendererComponent& rhs) {
				return lhs.Layer < rhs.Layer;
				});
			for (auto entity : group) {
				auto [sprite, transform] = group.get<SpriteRendererComponent, TransformComponent>(entity);
				if (sprite.Texture) { 
					Renderer2D::DrawQuad(transform.GetTransform(), std::make_shared<SpriteRendererComponent>(sprite), sprite.currFrame, sprite.totalFrames);
				}
				else {
					Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
				}
			}

			Renderer2D::EndScene();
		}
		else {
			Renderer2D::ClearRenderTarget();
			SE_CORE_WARN("No Scene Camera Detected!");
		}

		

		//Update Camera
		auto groupCamera = m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
		for (auto entity : groupCamera) {
			auto [camera, transform] = groupCamera.get<CameraComponent, TransformComponent>(entity);
			camera.Camera.m_Transform = transform.GetTransform();
			camera.Camera.UpdateView();
		}


		
	}


	void Scene::OnUpdateEditor(Timestep ts, Ref<SceneCamera> camera, bool draw_grid, Ref<Object> selected) {
		SetSceneCamera(camera);
		Renderer2D::BeginScene(camera.get());
		if (draw_grid) { Renderer2D::DrawBackgroundGrid(1); }
		

		auto animgroup = m_Registry.group<AnimationComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : animgroup) {
			auto [anim, sprite] = animgroup.get<AnimationComponent, SpriteRendererComponent>(entity);
			sprite.currFrame = anim.currframe;
			sprite.totalFrames = anim.frames;
		}


		

		auto group = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
		group.sort<SpriteRendererComponent>([](const SpriteRendererComponent& lhs, const SpriteRendererComponent& rhs) {
			return lhs.Layer < rhs.Layer;
			});
		for (auto entity : group) {
			auto [sprite, transform] = group.get<SpriteRendererComponent, TransformComponent>(entity);

			if (sprite.Texture) {	
				Renderer2D::DrawQuad(transform.GetTransform(), std::make_shared<SpriteRendererComponent>(sprite), sprite.currFrame, sprite.totalFrames);
			}
			else {
				Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
			}
		}

		auto groupCamera = m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
		for (auto entity : groupCamera) {
			auto [camera, transform] = groupCamera.get<CameraComponent, TransformComponent>(entity);
			glm::vec4 color = { 0.6f,0.6f,0.6f,1.0f };
			float size = camera.Camera.GetOrthographicSize();
			float height = (size / 2)*(Renderer2D::GetRenderTargetSize().x/Renderer2D::GetRenderTargetSize().y);
			Renderer2D::DrawBox({ -height,-size / 2 }, { -height,size / 2 }, { height,size / 2 }, { height,-size / 2 }, transform.GetTransform(), color);
			Renderer2D::DrawGizmo(transform.GetTransform(), Renderer2D::GetGizmo(), Renderer2D::GetGizmoColorInActive());
		}

		m_Registry.view<TransformComponent>().each([=](auto object, TransformComponent& tc) {
			if (selected) {
				if (*selected.get() == object) {
					glm::vec4 color = { 1.0f,0.5f,0.0f,1.0f };
					if (!Object(object, this).HasComponent<CameraComponent>()) {
						Renderer2D::DrawBox({ -0.5f,-0.5f }, { -0.5f,0.5f }, { 0.5f,0.5f }, { 0.5f, -0.5f }, tc.GetTransform(), color);
					}
					else {
						float size = Object(object, this).GetComponent<CameraComponent>().Camera.GetOrthographicSize();
						float height = (size / 2) * (Renderer2D::GetRenderTargetSize().x / Renderer2D::GetRenderTargetSize().y);
						Renderer2D::DrawBox({ -height,-size / 2 }, { -height,size / 2 }, { height,size / 2 }, { height,-size / 2 }, tc.GetTransform(), color);
						Renderer2D::DrawGizmo(tc.GetTransform(), Renderer2D::GetGizmo(), Renderer2D::GetGizmoColorActive());
					}
				}
			}
		});

		auto view = m_Registry.view<BoxColliderComponent>();
		for(auto o : view)
		{
			glm::vec4 color = { 0.0f,1.0f,0.0f,0.33f };
			if (selected) {
				if (*selected.get() == o) {
					color.a = 1.0f;
				}
			}

			Object object = { o, this };
			BoxColliderComponent bc = object.GetComponent<BoxColliderComponent>();
			TransformComponent tc = object.GetComponent<TransformComponent>();
			Renderer2D::DrawBox({ -bc.Size.x / 2 + bc.Offset.x,-bc.Size.y / 2 - bc.Offset.y }, { bc.Size.x / 2 + bc.Offset.x,-bc.Size.y / 2 - bc.Offset.y }, { bc.Size.x / 2 + bc.Offset.x,bc.Size.y / 2 - bc.Offset.y }, { -bc.Size.x / 2 + bc.Offset.x, bc.Size.y / 2 - bc.Offset.y }, tc.GetTransform(), color);
		}
		Renderer2D::EndScene();
	}


	unsigned int Scene::ObjectCount() {
		return (int)m_Registry.size();
	}

	Object Scene::CreateObject(const std::string& name)
	{
		std::string objName = (name.empty()) ? "NewGameObject" : name;
		entt::entity entity = m_Registry.create();
		Object object = { entity, this };
		object.AddComponent<TransformComponent>(object);
		object.AddComponent<TagComponent>(objName);
		return object;
	}

	Object Scene::CreateObject(const std::string& name, UUID uuid)
	{
		std::string objName = (name.empty()) ? "NewGameObject" : name;
		entt::entity entity = m_Registry.create();
		Object object = { entity, this };
		object.AddComponent<TransformComponent>(object);
		object.AddComponent<TagComponent>(objName, uuid);
		return object;
	}

	Object Scene::DuplicateObject(entt::entity o)
	{
		Object old = Object(o,this);
		auto& oldtc = old.GetComponent<TransformComponent>();
		Object duplicate = CreateObject(old.GetComponent<TagComponent>().Tag + "_dup", SurfEngine::UUID());
		duplicate.GetComponent<TransformComponent>().Scale = oldtc.Scale;
		duplicate.GetComponent<TransformComponent>().Rotation = oldtc.Rotation;
		duplicate.GetComponent<TransformComponent>().Translation = oldtc.Translation;
		if (old.HasComponent<CameraComponent>()) { duplicate.AddComponent<CameraComponent>(old.GetComponent<CameraComponent>()); }
		if (old.HasComponent<AnimationComponent>()) { duplicate.AddComponent<AnimationComponent>(old.GetComponent<AnimationComponent>()); }
		if (old.HasComponent<SpriteRendererComponent>()) { duplicate.AddComponent<SpriteRendererComponent>(old.GetComponent<SpriteRendererComponent>()); }
		if (old.HasComponent<ScriptComponent>()) { duplicate.AddComponent<ScriptComponent>(old.GetComponent<ScriptComponent>()); }
		if (old.HasComponent<BoxColliderComponent>()) { duplicate.AddComponent<BoxColliderComponent>(old.GetComponent<BoxColliderComponent>()); }
		if (old.HasComponent<RigidbodyComponent>()) { duplicate.AddComponent<RigidbodyComponent>(old.GetComponent<RigidbodyComponent>()); }

		return duplicate;
	}

	void Scene::DeleteObject(entt::entity o)
	{
		Object obj = Object(o, this);
		TransformComponent& tc = obj.GetComponent<TransformComponent>();
		
		for (TransformComponent* c : tc.children) {
			DeleteObject(c->gameObject);
		}

		if (tc.parent)
			tc.parent->RemoveChild(&tc);
		m_Registry.destroy(o);

	}



	void Scene::OnSceneEnd() {
		OnPhysics2DStop();
		m_IsPlaying = false;
		m_sceneCamera = nullptr;

		ScriptEngine::SceneEnd();

		SE_CORE_INFO("Scene \"" + m_name + ".scene\" Ended");
	}

	void Scene::OnSceneStart() {
		SE_CORE_INFO("Scene \"" + m_name + ".scene\" Started");

		m_Registry.view<AnimationComponent>().each([=](auto object, AnimationComponent& ac) {
			ac.play = ac.playOnAwake;
			});


		m_Registry.view<CameraComponent>().each([=](auto object, CameraComponent& cc) {
			m_sceneCamera = std::make_shared<SceneCamera>(cc.Camera);
			});

		ScriptEngine::SceneStart();
		OnPhysics2DStart();

	
		m_Registry.view<ScriptComponent>().each([&](auto object, ScriptComponent& cc) {
			ScriptEngine::SetCurrentScene(this);
			std::filesystem::path path = cc.path;
			cc.monoclass = ScriptClass("", path.stem().string().c_str());
			if (cc.monoclass.monoclass) {
				MonoClass* monoclassG = mono_class_get_parent(cc.monoclass.monoclass);
				MonoMethod* method_Constructor = mono_class_get_method_from_name(monoclassG, "SetGameObject", 1);
				cc.script_class_instance = cc.monoclass.CreateInstance();

				void* args[1];
				args[0] = ScriptEngine::CreateMonoString(Object(object, this).GetComponent<TagComponent>().uuid.ToString().c_str());
				cc.monoclass.InvokeMethod(cc.script_class_instance, method_Constructor, args);

				MonoClassField* field;
				void* iter = NULL;
				while ((field = mono_class_get_fields(cc.monoclass.monoclass, &iter))) {
					std::string name = mono_field_get_name(field);
					for (int i = 0; i < cc.variables.size(); i++) {
						if (cc.variables[i].name._Equal(name)) {
							if (cc.variables[i].type._Equal("double")) {
								double d = std::stod(cc.variables[i].user_value);
								mono_field_set_value(cc.script_class_instance, field, &d);
							}
							if (cc.variables[i].type._Equal("int")) {
								int i = std::stoi(cc.variables[i].user_value);
								mono_field_set_value(cc.script_class_instance, field, &i);
							}
							if (cc.variables[i].type._Equal("string")) {
								mono_field_set_value(cc.script_class_instance, field, &cc.variables[i].user_value);
							}
							if (cc.variables[i].type._Equal("bool")) {
								bool b = cc.variables[i].user_value._Equal("true");
								mono_field_set_value(cc.script_class_instance, field, &b);
							}
							break;
						}
					}
				}
				
				cc.monoclass.InvokeMethod(cc.script_class_instance, cc.monoclass.GetMethod("OnStart", 0));
			}
			else {
				SE_CORE_WARN("SCRIPT_ENGINE: Failed to Load Script [{0}]",path.filename().string());
			}

		});

		
		m_IsPlaying = true;

	}

	Object Scene::GetObjectByUUID(UUID uuid) {
		Object o;
		auto groupTag = m_Registry.group<TagComponent>(entt::get<TransformComponent>);
		m_Registry.view<TagComponent>().each([&](auto object, TagComponent& tc) {
				if (uuid ==  tc.uuid) {
					o = Object(object, this);
				}
			});
		return o;
	}

	Object Scene::GetObjectByName(std::string name) {
		Object o;
		auto groupTag = m_Registry.group<TagComponent>(entt::get<TransformComponent>);
		m_Registry.view<TagComponent>().each([&](auto object, TagComponent& tc) {
			if (name._Equal(tc.Tag)) {
				o = Object(object, this);
			}
		});
		return o;
	}

	void Scene::OnPhysics2DStart()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_Registry.view<RigidbodyComponent>();
		for (auto o : view)
		{
			Object object = { o, this };
			auto& transform = object.GetComponent<TransformComponent>();
			auto& rb2d = object.GetComponent<RigidbodyComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Translation.x, -transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			
			rb2d.RuntimeBody = body;

			if (object.HasComponent<BoxColliderComponent>())
			{
				auto& bc2d = object.GetComponent<BoxColliderComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x/2 * transform.Scale.x, bc2d.Size.y/2 * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnPhysics2DStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

}