#include "terrain_trianglelist.h"
#include "terrain.h"

void TriangleList::CreateTriangleList(int width, int depth, const Terrain* pTerrain) {
    mWidth = width;
    mDepth = depth;
    setupTriangleList(pTerrain);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void TriangleList::setupTriangleList(const Terrain* pTerrain) {
    Vertices.resize(mWidth * mDepth);
    int idx = 0;
    for (int z = 0; z < mDepth; z++)
        for (int x = 0; x < mWidth; x++) {
            assert(idx < Vertices.size());
            Vertices[idx++].InitVertex(pTerrain, x, z);
        }
    assert(idx == Vertices.size());

    int NumQuads = (mWidth - 1) * (mDepth - 1);
    Indices.resize(NumQuads * 6);
    idx = 0;
    for (int z = 0; z < mDepth - 1; z++) {
        for (int x = 0; x < mWidth - 1; x++) {
            unsigned int IndexBottomLeft = z * mWidth + x;
            unsigned int IndexTopLeft = (z + 1) * mWidth + x;
            unsigned int IndexTopRight = (z + 1) * mWidth + x + 1;
            unsigned int IndexBottomRight = z * mWidth + x + 1;

            // Add top left triangle
            assert(idx < Indices.size());
            Indices[idx++] = IndexBottomLeft;
            assert(idx < Indices.size());
            Indices[idx++] = IndexTopLeft;
            assert(idx < Indices.size());
            Indices[idx++] = IndexTopRight;

            // Add bottom right triangle
            assert(idx < Indices.size());
            Indices[idx++] = IndexBottomLeft;
            assert(idx < Indices.size());
            Indices[idx++] = IndexTopRight;
            assert(idx < Indices.size());
            Indices[idx++] = IndexBottomRight;
        }
    }
    assert(idx == Indices.size());

    glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(void*)offsetof(Vertex, Tex));
}

void TriangleList::Draw(Shader& shader) {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (mDepth - 1) * (mWidth - 1) * 6, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}

void TriangleList::Vertex::InitVertex(const Terrain* pTerrain, int x, int z) {
	float y = pTerrain->GetHeight(x, z);
	float WorldScale = pTerrain->GetWorldScale();
    Pos = glm::vec3(x * WorldScale, y, z * WorldScale);

    float sz = (float)pTerrain->getSize();
    float texScale = pTerrain->GetTexScale();
    Tex = glm::vec2(texScale * (float)x / sz, texScale * (float)z / sz);
}