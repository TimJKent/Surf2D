#pragma once

namespace SurfEngine {
	struct PhysicsMaterial {
		float Density = 1.0f;
		float Friction = 0.0f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;
	};
}