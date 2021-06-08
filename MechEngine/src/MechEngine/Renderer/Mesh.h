#pragma once
#include "glm/glm.hpp"
#include "MechEngine/Core/Color.h"

namespace MechEngine {
	class Mesh {
	public:
		Mesh();
		Mesh(const std::string& filePath);
		~Mesh();
		
		void AddVertex(glm::vec3 vertexPosition, glm::vec2 texturePosition, Color color);
		void AddIndex(int value);

		void SetIndex(int slot, int value);
		void SetPosition(int slot, glm::vec3 position);
		void SetTexture(int slot, glm::vec2 position);
		void SetColor(int slot, Color color);

		uint32_t GetIndex(int slot);
		glm::vec3 GetVertexPosition(int slot);
		glm::vec2 GetTexturePosition(int slot);
		Color GetColor(int slot);

		int Size() { return m_VertexPositions.size(); }
		int IndicesSize() { return m_Index.size(); }
		//probably needs fixed later this prob looks so random.
		int GetStride() { return 9; }

		uint32_t* GetIndices() { return &m_Index[0]; }
		float* GetVertices();

		


	private:
		std::vector<uint32_t> m_Index;
		std::vector<glm::vec3> m_VertexPositions;
		std::vector<glm::vec2> m_TexturePositions;
		std::vector<Color> m_ColorData;
	};
}