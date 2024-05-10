#ifndef __A_MESH_H__
#define __A_MESH_H__

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "render/shader.h"

#define MAX_BONE_INFLUENCE 4

struct Materials {
    int id;
    std::string type;
    std::string path;
    // if mesh has color only, id = -1
    glm::vec3 color;
};

class aMesh {
public:
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
		//bone indexes which will influence this vertex
		int m_BoneIDs[MAX_BONE_INFLUENCE];
		//weights from each bone
		float m_Weights[MAX_BONE_INFLUENCE];
	};

	std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Materials> materials;
	unsigned int VAO;
	std::vector<glm::vec3> mPositions;
    std::unordered_map<std::string, std::vector<glm::vec3>> morphAnims;


	aMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Materials> materials);
    aMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Materials> materials, 
	std::unordered_map<std::string, std::vector<glm::vec3>> morphAnims);

    void Draw(Shader &shader);
    void Draw(Shader &shader, std::unordered_map<std::string, float> morphanimkeys);
    void DrawInstance(Shader &shader);

private:
	unsigned int VBO, EBO;
	unsigned int VBO_Position;

	void setupMesh();
};

#endif // !__A_MESH_H__