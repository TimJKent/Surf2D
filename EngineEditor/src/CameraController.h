#pragma once

#include "SurfEngine/Scenes/ScriptableObject.h"

namespace SurfEngine {
	class CameraController : public ScriptableObject {
	public:
		void OnUpdate(Timestep ts) {
			auto& transform = GetComponent<TransformComponent>();
			float speed = -5.0f;

			if (Input::IsKeyPressed(SE_KEY_W)) {
				transform.Translation.y += speed * ts;
			}
			if (Input::IsKeyPressed(SE_KEY_A)) {
				transform.Translation.x += speed * ts;
			}
			if (Input::IsKeyPressed(SE_KEY_S)) {
				transform.Translation.y -= speed * ts;
			}
			if (Input::IsKeyPressed(SE_KEY_D)) {
				transform.Translation.x -= speed * ts;
			}
		}
	};
}

