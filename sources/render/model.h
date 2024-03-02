#ifndef __MODEL_H__
#define __MODEL_H__

#include "mesh.h"
#include "shader.h"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

class Model {
public:
	// model data
	vector<Texture> textures_loaded;
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;

	Model(string const& path, bool gamma = false) : gammaCorrection(gamma) {
		loadModel(path);
	}
	void Draw(Shader& shader) {
		for (unsigned int i = 0; i < meshes.size(); ++i) meshes[i].Draw(shader);
	}

private:
	void loadModel(string const &path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};

unsigned int TextureFromFile(const char* path, const string& directory);

#endif // !__MODEL_H__
