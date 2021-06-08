#include "mepch.h"
#include "Shader.h"

#include "Renderer.h"
#include "MechEngine/Platform/OpenGl/OpenGLShader.h"

namespace MechEngine {
	Ref<Shader> Shader::Create(const std::string& filepath) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: ME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(filepath);
		}
		ME_CORE_ASSERT(false, "Unknown RendererAPI specified!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None: ME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}
		ME_CORE_ASSERT(false, "Unknown RendererAPI specified!");
		return nullptr;
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader) {
		ME_CORE_ASSERT(!Exists(name), "Shader already exists: {0}!", name);
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader) {
		auto&name = shader->GetName();
		Add(name, shader);
	}

	MechEngine::Ref<MechEngine::Shader> ShaderLibrary::Load(const std::string& filepath) {
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}
	
	MechEngine::Ref<MechEngine::Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath) {
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	MechEngine::Ref<MechEngine::Shader> ShaderLibrary::Get(const std::string& name) {
		ME_CORE_ASSERT(Exists(name), "Shader not found {0}!", name);
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) {
		return m_Shaders.find(name) != m_Shaders.end();
	}
}

