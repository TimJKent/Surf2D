#include "mepch.h"
#include "Mesh.h"
#include <fstream>

namespace MechEngine {
	Mesh::Mesh() {
		
	}

	Mesh::~Mesh() {
		
	}

	void Mesh::AddVertex(glm::vec3 vertexPosition, glm::vec2 texturePosition, Color color) {
		m_VertexPositions.push_back(vertexPosition);
		m_TexturePositions.push_back(texturePosition);
		m_ColorData.push_back(color);
	}

	void Mesh::AddIndex(int value) {
		m_Index.push_back(value);
	}

	void Mesh::SetIndex(int slot, int value) {
		m_Index[slot] = value;
	}

	void Mesh::SetPosition(int slot, glm::vec3 position) {
		m_VertexPositions[slot] = position;
	}

	void Mesh::SetTexture(int slot, glm::vec2 position) {
		m_TexturePositions[slot] = position;
	}

	void Mesh::SetColor(int slot, Color color) {
		m_ColorData[slot] = color;
	}

	glm::vec3 Mesh::GetVertexPosition(int slot) {
		return m_VertexPositions[slot];
	}

	glm::vec2 Mesh::GetTexturePosition(int slot) {
		return m_TexturePositions[slot];

	}

	Color Mesh::GetColor(int slot) {
		return m_ColorData[slot];
	}

	uint32_t Mesh::GetIndex(int slot) {
		return m_Index[slot];
	}

	float* Mesh::GetVertices() {
		std::vector<float> outputVertices;

		for (int i = 0; i < Size(); i++) {
			glm::vec3 pos = m_VertexPositions[i];
			glm::vec2 tex = m_TexturePositions[i];
            glm::vec4 color = m_ColorData[i].GetChannelRGBA();
			outputVertices.push_back(pos.x);
			outputVertices.push_back(pos.y);
			outputVertices.push_back(pos.z);
			outputVertices.push_back(tex.x);
			outputVertices.push_back(tex.y);
            outputVertices.push_back(color.r);
            outputVertices.push_back(color.g);
            outputVertices.push_back(color.b);
            outputVertices.push_back(color.a);
		}
		return &outputVertices[0];
	}


     Mesh::Mesh(const std::string& filePath) {

        std::vector<glm::vec3> Vertexs;
        std::vector<glm::vec2> Textures;

        std::cout << "Parsing .obj file at " << filePath << std::endl;
        std::ifstream stream(filePath.c_str());

        ME_ASSERT(stream.is_open(),"ERROR: Failed to open");

        
        std::string str_line;
        int counter = 0;
        while (getline(stream, str_line)) {
            std::string_view line{ str_line };
            if (line.find("v ") != std::string::npos) {
                std::stringstream ss(line.substr(2, line.length()).data());
                glm::vec3 vd;
                ss >> vd.x;
                ss >> vd.y;
                ss >> vd.z;
                Vertexs.push_back(vd);
            }
            else if (line.find("vn ") != std::string::npos) {
                //Normals if i ever do lighting ¯\_(ツ)_/¯
            }
            else if (line.find("vt ") != std::string::npos) {
                std::stringstream ss(line.substr(2, line.length()).data());
                glm::vec2 td;
                ss >> td.x;
                ss >> td.y;
                Textures.push_back(td);
            }
            else  if (line.find("f ") != std::string::npos) {
                glm::vec3 id1;
                glm::vec3 id2;
                glm::vec3 id3;
                std::stringstream ss(line.substr(2, line.length()).data());
                ss >> id1.x;
                ss.ignore(1, '/');
                ss >> id1.y;
                ss.ignore(1, '/');
                ss >> id1.z;

                ss >> id2.x;
                ss.ignore(1, '/');
                ss >> id2.y;
                ss.ignore(1, '/');
                ss >> id2.z;

                ss >> id3.x;
                ss.ignore(1, '/');
                ss >> id3.y;
                ss.ignore(1, '/');
                ss >> id3.z;

                AddVertex(Vertexs[id2.x-1], Textures[id2.y-1], Color(1.f,1.f,1.f,1.f));
                AddVertex(Vertexs[id3.x-1], Textures[id3.y-1], Color(1.f,1.f,1.f,1.f));
                AddVertex(Vertexs[id1.x-1], Textures[id1.y-1], Color(1.f,1.f,1.f,1.f));

                m_Index.push_back(counter++);
                m_Index.push_back(counter++);
                m_Index.push_back(counter++);
            }
        }
        std::cout << "Completed Parsing" << std::endl;
    }

}
