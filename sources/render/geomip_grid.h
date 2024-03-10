#ifndef __GEOMIP_GRID_H__
#define __GEOMIP_GRID_H__

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glad/gl.h>

#include "lod_manager.h"

using namespace std;

class Terrain;

class GeoMipGrid {
public:
    GeoMipGrid() {};
    ~GeoMipGrid() = default;
    void Create(int w, int d, int patchSize, const Terrain* pterrain);
    void Draw(glm::vec3 CameraPos);
    void Destroy() {
        if (VAO > 0) glDeleteVertexArrays(1, &VAO);
        if (VBO > 0) glDeleteBuffers(1, &VBO);
        if (EBO > 0) glDeleteBuffers(1, &EBO);
    }

private:
    typedef struct Vertex {
        glm::vec3 Pos;
        glm::vec2 Tex;
        glm::vec3 Normal = glm::vec3(0.0f, 0.0f, 0.0f);
        void InitVertex(const Terrain* pTerrain, int x, int z);
    } Vertex;

    void setupGrid(const Terrain* pterrain);
    unsigned int AddTriangle(unsigned int Index, std::vector<unsigned int>& Indices, 
                            unsigned int v1, unsigned int v2, unsigned int v3);
    void CalcNormals(std::vector<Vertex>& Vertices, std::vector<unsigned int>& Indices);
    void InitVertices(const Terrain* pTerrain, std::vector<Vertex>& Vertices);
    int InitIndices(std::vector<unsigned int>& Indices);
    int InitIndicesLOD(int Index, std::vector<unsigned int>& Indices, int lod);
    int InitIndicesLODSingle(int Index, std::vector<unsigned int>& Indices, 
            int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom);
    unsigned int CreateTriangleFan(int Index, std::vector<unsigned int>& Indices, 
            int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom, int x, int z);
    int CalcNumIndices();

    int mWidth = 0;
    int mDepth = 0;
    int mPatchSize = 0;
    unsigned int VAO, VBO, EBO;

    int mMaxLOD = 0;
    LODManager mLodManager;
    typedef struct SingleLodInfo {
        int start = 0;
        int count = 0;
    } SingleLodInfo;
    #define LEFT   2
    #define RIGHT  2
    #define TOP    2
    #define BOTTOM 2
    typedef struct LodInfo {
        SingleLodInfo info[LEFT][RIGHT][TOP][BOTTOM];
    } LodInfo;
    vector<LodInfo> mLodInfo;
    int mNumPatchesX = 0;
    int mNumPatchesZ = 0;
};

#endif // !__GEOMIP_GRID_H__