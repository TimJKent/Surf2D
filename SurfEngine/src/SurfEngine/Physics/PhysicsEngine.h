#pragma once
#include "glm/glm.hpp"

class b2World;

namespace SurfEngine {
	class Scene;
	class Timestep;

	struct PhysicsData {
		glm::vec2 gravity_scale = {0.0f,-9.8f};
		uint32_t velocity_iterations = 6;
		uint32_t position_iterations = 2;
	};

	class PhysicsEngine
	{
	public:
		static void OnPhysics2DStart(Scene* scene);
		static void OnPhysics2DStop();
		static void OnPhysics2DUpdate(Timestep ts, Scene* scene);
	public:
		static PhysicsData s_Data;
	private:
		static b2World* m_PhysicsWorld;
	};
}