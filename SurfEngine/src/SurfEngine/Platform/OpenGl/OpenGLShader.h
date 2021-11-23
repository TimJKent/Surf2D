#pragma once
#include "SurfEngine/Renderer/Shader.h"
#include <glm/glm.hpp>

typedef unsigned int GLenum;

namespace SurfEngine {
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		void Bind() const override;
		void Unbind() const override;

		virtual void SetFloat(const std::string& name, const float value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;
		virtual void SetInt(const std::string& name, const int value) override;
		virtual void SetInt(const std::string& name, const size_t value) override;

		virtual const std::string& GetName() const override { return m_Name; };

		void UploadUniformInt(const std::string& name, const uint32_t num);
		void UploadUniformFloat(const std::string& name, const float num);
		void UploadUniformVec3(const std::string& name, const glm::vec3 vector);
		void UploadUniformVec4(const std::string& name, const glm::vec4 vector);
		void UploadUniformMat4(const std::string& name, const glm::mat4 matrix);
	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		std::string m_Name;
		uint32_t m_RendererID;
	};
}


