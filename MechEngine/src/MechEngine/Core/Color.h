#pragma once

#pragma once
#include "glm/glm.hpp"

namespace MechEngine {
	class Color {
	public:

		//Constructors
		Color() {
			m_Color =
			{
				1.f,1.f,1.f,1.f
			};
		}

		Color(glm::vec3 color) { 
			m_Color = 
			{ 
				glm::clamp(color, {0.f,0.f,0.f}, {1.f,1.f,1.f}), 
				1.0f 
			}; 
		}

		bool operator==(const Color& c) {
			return (this->m_Color.r == c.m_Color.r) 
				&& (this->m_Color.g == c.m_Color.g)
				&& (this->m_Color.b == c.m_Color.b)
				&& (this->m_Color.a == c.m_Color.a);
		}

		
		Color(glm::vec4 color) { 
			m_Color = glm::clamp(color, { 0.f,0.f,0.f,0.f }, { 1.f,1.f,1.f,1.f }); 
		}
		
		Color(float r, float g, float b) { 
			m_Color = 
			{
				glm::clamp(r,0.f,1.f),
				glm::clamp(g,0.f,1.f),
				glm::clamp(b,0.f,1.f),
				1.0f 
			}; 
		}

		Color(float r, float g, float b,float a) {
			m_Color = 
			{
				glm::clamp(r,0.f,1.f),
				glm::clamp(g,0.f,1.f),
				glm::clamp(b,0.f,1.f),
				glm::clamp(a,0.f,1.f)
			};
		}

		//Setters
		void SetChannelRed(float value) {
			m_Color.r = glm::clamp(value, 0.f, 1.f);
		}

		void SetChannelGreen(float value) {
			m_Color.g = glm::clamp(value, 0.f, 1.f);
		}

		void SetChannelBlue(float value) {
			m_Color.b = glm::clamp(value, 0.f, 1.f);
		}

		void SetChannelAlpha(float value) {
			m_Color.a = glm::clamp(value, 0.f, 1.f);
		}

		//Getters
		float GetChannelRed() { return m_Color.r; }

		float GetChannelGreen() { return m_Color.g; }

		float GetChannelBlue() { return m_Color.b; }

		float GetChannelAlpha() { return m_Color.a; }

		glm::vec3 GetChannelRGB() { return m_Color; }

		glm::vec4 GetChannelRGBA() { return m_Color; }

		float* GetAsFloat3() {
			float out[3] = { m_Color.r,m_Color.g, m_Color.b };
			return out;
		}

		float* GetAsFloat4() {
			float out[4] = { m_Color.r,m_Color.g, m_Color.b, m_Color.a };
			return out;
		}

	private:
		glm::vec4 m_Color;
	};
}