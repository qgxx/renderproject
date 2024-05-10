#ifndef __A_MODEL_H__
#define __A_MODEL_H__

#include <glad/glad.h>
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
#include <thread>
#include <mutex>
#include "mesh.h"
#include "render/shader.h"
#include "bone.h"

struct TetxtureType
{
    aiTextureType type;
    std::string prefix;
};

class aModel {
public:
    // constructor, expects a filepath to a 3D model.
    aModel(std::string const &path, bool gamma = false);
    aModel(const aiScene *scene, const std::string path);
    // draws the model, and thus all its meshes
    void Draw(Shader &shader);
    void DrawInstance(Shader &shader);
    auto &GetBoneInfoMap() { return m_BoneInfoMap; }
    int &GetBoneCount() { return m_BoneCounter; }

    void SetMorphAnimKeys(std::unordered_map<std::string, float> morphanimkeys) { morphAnimKeys = morphanimkeys; }

    std::unordered_map<unsigned int, std::string> shapeKeysNameID;

private:
    // model data
    std::vector<Materials> m_materials; // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<aMesh> m_meshes;
    std::string directory;
    bool gammaCorrection;

    // morph data
    std::unordered_map<std::string, float> morphAnimKeys;
    // skelatal animation data
    std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

    void SetVertexBoneDataToDefault(aMesh::Vertex &vertex);
    void SetVertexBoneData(aMesh::Vertex &vertex, int boneID, float weight);
    void ExtractBoneWeightForVertices(std::vector<aMesh::Vertex> &vertices, aiMesh *mesh);

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes int the meshse vector.
    void loadModel(std::string const &path);
    // processes a node in a recursive. Processes each individual mesh located at the node and repeats this process on its children nodes
    void processNode(aiNode *node, const aiScene *scene);
    aMesh processMesh(aiMesh *mesh, const aiScene *scene);
    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    void loadMaterialTextures(std::vector<Materials> &materials, aiMaterial *mat, aiTextureType type, std::string typeName);

};

#endif // !__A_MODEL_H__