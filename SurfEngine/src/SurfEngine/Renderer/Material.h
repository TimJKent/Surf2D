#pragma once
#include "sepch.h"
#include "SurfEngine/Renderer/Shader.h"
#include "SurfEngine/Renderer/Texture.h"

namespace SurfEngine {
	class Material {
	public:
		static Ref<Material> Create() {
			return std::make_shared<Material>();
		}
	public:
		Material() {}
		~Material() {}

		void Bind() { 
			m_Shader->Bind();
			for (int i = 0; i < m_Textures.size(); i++) {
				m_Textures[i]->Bind(i);
			}
		}

		void Unbind() { m_Shader->Unbind(); }

		void AddTexture(const Ref<Texture>& texture) {
			m_Shader->Bind();
			m_Shader->SetInt("u_Texture", (int)m_Textures.size());
			m_Textures.push_back(texture);
		}

		void SetShader(const std::string& shader_path) { m_Shader = Shader::Create(shader_path); }
		void SetShader(const Ref<Shader>& shader) { m_Shader = shader; }
		Ref<Shader> GetShader() { return m_Shader; }

		std::vector<Ref<Texture>> m_Textures;
	private:
		Ref<Shader> m_Shader;
	};
}