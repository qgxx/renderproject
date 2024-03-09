#include "geomip_grid.h"
#include "terrain.h"

void GeoMipGrid::Create(int w, int d, int PatchSize, const Terrain* pterrain) {
    if ((w - 1) % (PatchSize - 1) != 0) {
        int RecommendedWidth = ((w - 1 + PatchSize - 1) / (PatchSize - 1)) * (PatchSize - 1) + 1;
        printf("Width minus 1 (%d) must be divisible by PatchSize minus 1 (%d)\n", w, PatchSize);
        printf("Try using Width = %d\n", RecommendedWidth);
        exit(0);
    }

    if ((d - 1) % (PatchSize - 1) != 0) {
        int RecommendedDepth = ((d - 1 + PatchSize - 1) / (PatchSize - 1)) * (PatchSize - 1) + 1;
        printf("Depth minus 1 (%d) must be divisible by PatchSize minus 1 (%d)\n", d, PatchSize);
        printf("Try using Width = %d\n", RecommendedDepth);
        exit(0);
    }

    if (PatchSize < 3) {
        printf("The minimum patch size is 3 (%d)\n", PatchSize);
        exit(0);
    }

    if (PatchSize % 2 == 0) {
        printf("Patch size must be an odd number (%d)\n", PatchSize);
        exit(0);
    }

    mWidth = w;
    mDepth = d;
    mPatchSize = PatchSize;

    setupGrid(pterrain);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GeoMipGrid::setupGrid(const Terrain* pTerrain) {
    std::vector<Vertex> Vertices;
    Vertices.resize(mWidth * mDepth);
    int Index = 0;
    for (int z = 0 ; z < mDepth ; z++) {
        for (int x = 0 ; x < mWidth ; x++) {
            assert(Index < Vertices.size());
        Vertices[Index].InitVertex(pTerrain, x, z);
        Index++;
        }
    }
    assert(Index == Vertices.size());

    std::vector<unsigned int> Indices;
    int NumQuads = (mPatchSize - 1) * (mPatchSize - 1);
    Indices.resize(NumQuads * 6);
    Index = 0;
    for (int z = 0 ; z < mPatchSize - 1 ; z += 2) {
        for (int x = 0 ; x < mPatchSize - 1 ; x += 2) {
            unsigned int IndexCenter = (z + 1) * mWidth + x + 1;
            unsigned int IndexTemp1 = z * mWidth + x;
            unsigned int IndexTemp2 = (z + 1) * mWidth + x;

            Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

            IndexTemp1 = IndexTemp2;
            IndexTemp2 += mWidth;

            Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

            IndexTemp1 = IndexTemp2;
            IndexTemp2++;

            Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

            IndexTemp1 = IndexTemp2;
            IndexTemp2++;

            Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

            IndexTemp1 = IndexTemp2;
            IndexTemp2 -= mWidth;

            Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

            IndexTemp1 = IndexTemp2;
            IndexTemp2 -= mWidth;

            Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

            IndexTemp1 = IndexTemp2;
            IndexTemp2--;

            Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

            IndexTemp1 = IndexTemp2;
            IndexTemp2--;

            Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);
        }
    }
    assert(Index == Indices.size());

    CalcNormals(Vertices, Indices);

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
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(void*)offsetof(Vertex, Normal));
}

unsigned int GeoMipGrid::AddTriangle(unsigned int Index, std::vector<unsigned int>& Indices, 
                                    unsigned int v1, unsigned int v2, unsigned int v3) {
    //printf("Add triangle %d %d %d\n", v1, v2, v3);
    assert(Index < Indices.size());
    Indices[Index++] = v1;
    assert(Index < Indices.size());
    Indices[Index++] = v2;
    assert(Index < Indices.size());
    Indices[Index++] = v3;

    return Index;
}

void GeoMipGrid::CalcNormals(std::vector<Vertex>& Vertices, std::vector<unsigned int>& Indices) {
    unsigned int Index = 0;

    // Accumulate each triangle normal into each of the triangle vertices
    for (int z = 0 ; z < mDepth - 1 ; z += (mPatchSize - 1)) {
        for (int x = 0 ; x < mWidth - 1; x += (mPatchSize - 1)) {
            int BaseVertex = z * mWidth + x;
            //printf("Base index %d\n", BaseVertex);
    		for (unsigned int i = 0 ; i < Indices.size() ; i += 3) {
                unsigned int Index0 = BaseVertex + Indices[i];
                unsigned int Index1 = BaseVertex + Indices[i + 1];
                unsigned int Index2 = BaseVertex + Indices[i + 2];
		        glm::vec3 v1 = Vertices[Index1].Pos - Vertices[Index0].Pos;
		        glm::vec3 v2 = Vertices[Index2].Pos - Vertices[Index0].Pos;
		        glm::vec3 Normal = glm::cross(v1, v2);
		        Normal = glm::normalize(Normal);

		        Vertices[Index0].Normal += Normal;
		        Vertices[Index1].Normal += Normal;
		        Vertices[Index2].Normal += Normal;
    		}
        }
    }

    // Normalize all the vertex normals
    for (unsigned int i = 0 ; i < Vertices.size() ; i++) {
        Vertices[i].Normal = glm::normalize(Vertices[i].Normal);
    }
}

void GeoMipGrid::Vertex::InitVertex(const Terrain* pTerrain, int x, int z) {
	float y = pTerrain->GetHeight(x, z);
	float WorldScale = pTerrain->GetWorldScale();
    Pos = glm::vec3(x * WorldScale, y, z * WorldScale);

    float sz = (float)pTerrain->getSize();
    float texScale = pTerrain->GetTexScale();
    Tex = glm::vec2(texScale * (float)x / sz, texScale * (float)z / sz);
}

void GeoMipGrid::Draw() {
    glBindVertexArray(VAO);
    for (int z = 0 ; z < mDepth - 1 ; z += (mPatchSize - 1)) {
        for (int x = 0 ; x < mWidth - 1 ; x += (mPatchSize - 1)) {
            int BaseVertex = z * mWidth + x;
            glDrawElementsBaseVertex(GL_TRIANGLES, (mPatchSize - 1) * (mPatchSize - 1) * 6, GL_UNSIGNED_INT, NULL, BaseVertex);
        }
    }
    glBindVertexArray(0);
}