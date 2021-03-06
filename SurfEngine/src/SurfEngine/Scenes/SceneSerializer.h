#pragma once
#include "Scene.h"

namespace SurfEngine {
	class SceneSerializer {
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialze(const std::string& filepath);
		void SerialzeRuntime(const std::string& filepath);

		bool Deserialze(const std::string& filepath);
		bool DeserialzeRuntime(const std::string& filepath);

	private:
		Ref<Scene> m_Scene;
	};
}