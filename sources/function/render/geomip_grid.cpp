#include "geomip_grid.h"
#include "terrain.h"

extern int gShowPoints;

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

    mNumPatchesX = (w - 1) / (PatchSize - 1);
    mNumPatchesZ = (d - 1) / (PatchSize - 1);
    float WorldScale = pterrain->GetWorldScale();
    mMaxLOD = mLodManager.InitLodManager(PatchSize, mNumPatchesX, mNumPatchesZ, WorldScale);
    mLodInfo.resize(mMaxLOD + 1);

    setupGrid(pterrain);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_Terrain = pterrain;
    mWorldScale = m_Terrain->GetWorldScale();
}

void GeoMipGrid::setupGrid(const Terrain* pTerrain) {
    std::vector<Vertex> Vertices;
    Vertices.resize(mWidth * mDepth);
    printf("Preparing space for %zu vertices\n", Vertices.size());
    InitVertices(pTerrain, Vertices);

    int NumIndices = CalcNumIndices();
    std::vector<unsigned int> Indices;
    Indices.resize(NumIndices);
    NumIndices = InitIndices(Indices);
    printf("Final number of indices %d\n", NumIndices);

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
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tex));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
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
            int numIndices = mLodInfo[0].info[0][0][0][0].count;
    		for (unsigned int i = 0 ; i < numIndices; i += 3) {
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

void GeoMipGrid::Draw(const glm::vec3 CameraPos) {
    mLodManager.Update(CameraPos);

    glBindVertexArray(VAO);
    if (gShowPoints > 0) {
        glDrawElementsBaseVertex(GL_POINTS, mLodInfo[0].info[0][0][0][0].count, GL_UNSIGNED_INT, (void*)0, 0);
    }

    if (gShowPoints != 2) {
        for (int PatchZ = 0 ; PatchZ < mNumPatchesZ ; PatchZ++) {
            for (int PatchX = 0 ; PatchX < mNumPatchesX ; PatchX++) {
                int z = PatchZ * (mPatchSize - 1);
                int x = PatchX * (mPatchSize - 1);
                
                const LODManager::PatchLod& plod = mLodManager.GetPatchLod(PatchX, PatchZ);
                int C = plod.Core;
                int L = plod.Left;
                int R = plod.Right;
                int T = plod.Top;
                int B = plod.Bottom;

                size_t BaseIndex = sizeof(unsigned int) * mLodInfo[C].info[L][R][T][B].start;

                int BaseVertex = z * mWidth + x;
                //            printf("%d\n", BaseVertex);

                glDrawElementsBaseVertex(GL_TRIANGLES, mLodInfo[C].info[L][R][T][B].count, 
                                        GL_UNSIGNED_INT, (void*)BaseIndex, BaseVertex);
            }
        }
    }
}

void GeoMipGrid::InitVertices(const Terrain* pTerrain, std::vector<Vertex>& Vertices) {
    int Index = 0;

    for (int z = 0 ; z < mDepth ; z++) {
        for (int x = 0 ; x < mWidth ; x++) {
            assert(Index < Vertices.size());
			Vertices[Index].InitVertex(pTerrain, x, z);
			Index++;
        }
    }

    assert(Index == Vertices.size());
}


int GeoMipGrid::InitIndices(std::vector<unsigned int>& Indices) {
    int Index = 0;

    for (int lod = 0 ; lod <= mMaxLOD ; lod++) {
        printf("*** Init indices lod %d ***\n", lod);
        Index = InitIndicesLOD(Index, Indices, lod);
        printf("\n");
    }

    return Index;
}

int GeoMipGrid::InitIndicesLOD(int Index, std::vector<unsigned int>& Indices, int lod) {
    int TotalIndicesForLOD = 0;

    for (int l = 0 ; l < LEFT ; l++) {
        for (int r = 0 ; r < RIGHT ; r++) {
            for (int t = 0 ; t < TOP ; t++) {
                for (int b = 0 ; b < BOTTOM ; b++) {
                    mLodInfo[lod].info[l][r][t][b].start = Index;
                    Index = InitIndicesLODSingle(Index, Indices, lod, lod + l, lod + r, lod + t, lod + b);

                    mLodInfo[lod].info[l][r][t][b].count = Index - mLodInfo[lod].info[l][r][t][b].start;
                    TotalIndicesForLOD += mLodInfo[lod].info[l][r][t][b].count;
                }
            }
        }
    }

    printf("Total indices for LOD: %d\n", TotalIndicesForLOD);

    return Index;
}


int GeoMipGrid::InitIndicesLODSingle(int Index, std::vector<unsigned int>& Indices, 
            int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom) {
    int FanStep = powi(2, lodCore + 1);   // lod = 0 --> 2, lod = 1 --> 4, lod = 2 --> 8, etc
    int EndPos = mPatchSize - 1 - FanStep;  // patch size 5, fan step 2 --> EndPos = 2; patch size 9, fan step 2 --> EndPos = 6

    for (int z = 0 ; z <= EndPos ; z += FanStep) {
        for (int x = 0 ; x <= EndPos ; x += FanStep) {
            int lLeft   = x == 0      ? lodLeft : lodCore;
            int lRight  = x == EndPos ? lodRight : lodCore;
            int lBottom = z == 0      ? lodBottom : lodCore;
            int lTop    = z == EndPos ? lodTop : lodCore;

            Index = CreateTriangleFan(Index, Indices, lodCore, lLeft, lRight, lTop, lBottom, x, z);
        }
    }

    return Index;
}

unsigned int GeoMipGrid::CreateTriangleFan(int Index, std::vector<unsigned int>& Indices, 
        int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom, int x, int z) {
    int StepLeft   = powi(2, lodLeft); // because LOD starts at zero...
    int StepRight  = powi(2, lodRight);
    int StepTop    = powi(2, lodTop);
    int StepBottom = powi(2, lodBottom);
    int StepCenter = powi(2, lodCore);

    unsigned int IndexCenter = (z + StepCenter) * mWidth + x + StepCenter;

    // first up
    unsigned int IndexTemp1 = z * mWidth + x;
    unsigned int IndexTemp2 = (z + StepLeft) * mWidth + x;

    Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

    // second up
    if (lodLeft == lodCore) {
        IndexTemp1 = IndexTemp2;
        IndexTemp2 += StepLeft * mWidth;

        Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);
    }

    // first right
    IndexTemp1 = IndexTemp2;
    IndexTemp2 += StepTop;

    Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

    // second right
    if (lodTop == lodCore) {
        IndexTemp1 = IndexTemp2;
        IndexTemp2 += StepTop;

        Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);
    }

    // first down
    IndexTemp1 = IndexTemp2;
    IndexTemp2 -= StepRight * mWidth;

    Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

    // second down
    if (lodRight == lodCore) {
        IndexTemp1 = IndexTemp2;
        IndexTemp2 -= StepRight * mWidth;

        Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);
    }

    // first left
    IndexTemp1 = IndexTemp2;
    IndexTemp2 -= StepBottom;

    Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

    // second left
    if (lodBottom == lodCore) {
        IndexTemp1 = IndexTemp2;
        IndexTemp2 -= StepBottom;

        Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);
    }

    return Index;
}

int GeoMipGrid::CalcNumIndices() {
    int NumQuads = (mPatchSize - 1) * (mPatchSize - 1);
    int NumIndices = 0;
    int MaxPermutationsPerLevel = 16;    // true/false for each of the four sides
    const int IndicesPerQuad = 6;        // two triangles
    for (int lod = 0; lod <= mMaxLOD; lod++) {
        printf("LOD %d: num quads %d\n", lod, NumQuads);
        NumIndices += NumQuads * IndicesPerQuad * MaxPermutationsPerLevel;
        NumQuads /= 4;
    }
    printf("Initial number of indices %d\n", NumIndices);
    return NumIndices;
}
