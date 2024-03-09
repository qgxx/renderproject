#ifndef __GEOMIP_GRID_H__
#define __GEOMIP_GRID_H__

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glad/gl.h>

class Terrain;

class GeoMipGrid {
public:
    GeoMipGrid() {};
    ~GeoMipGrid() = default;
    void Create(int w, int d, int patchSize, const Terrain* pterrain);
    void Draw();
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

    int mWidth = 0;
    int mDepth = 0;
    int mPatchSize = 0;
    unsigned int VAO, VBO, EBO;

};

#endif // !__GEOMIP_GRID_H__