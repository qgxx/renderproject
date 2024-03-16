#include "lod_manager.h"

int LODManager::InitLodManager(int PatchSize, int NumPatchesX, int NumPatchesZ, float WorldScale) {
    m_patchSize = PatchSize;
    m_numPatchesX = NumPatchesX;
    m_numPatchesZ = NumPatchesZ;
    m_worldScale = WorldScale;

    CalcMaxLOD();

    PatchLod Zero;
    m_map.set_all(NumPatchesX, NumPatchesZ, Zero);

    m_regions.resize(m_maxLOD + 1);

    CalcLodRegions();

    return m_maxLOD;
}


void LODManager::CalcMaxLOD() {
    int NumSegments = m_patchSize - 1;
    if (ceilf(log2f((float)NumSegments)) != floorf(log2f((float)NumSegments))) {
        printf("The number of vertices in the patch minus one must be a power of two\n");
        printf("%f %f\n", ceilf(log2f((float)NumSegments)), floorf(log2f((float)NumSegments)));
        exit(0);
    }

    int patchSizeLog2 = (int)log2f((float)NumSegments);
    printf("log2 of patch size %d is %d\n", m_patchSize, patchSizeLog2);
    m_maxLOD = patchSizeLog2 - 1;

   // printf("max lod %d\n", m_maxLOD);
}


void LODManager::Update(const glm::vec3& CameraPos) {
    UpdateLodMapPass1(CameraPos);
    UpdateLodMapPass2(CameraPos);
}


void LODManager::UpdateLodMapPass1(const glm::vec3& CameraPos) {
    int CenterStep = m_patchSize / 2;

    for (int LodMapZ = 0 ; LodMapZ < m_numPatchesZ ; LodMapZ++) {
        for (int LodMapX = 0 ; LodMapX < m_numPatchesX ; LodMapX++) {
            int x = LodMapX * (m_patchSize - 1) + CenterStep;
            int z = LodMapZ * (m_patchSize - 1) + CenterStep;

            glm::vec3 PatchCenter = glm::vec3(x * (float)m_worldScale, 0.0f, z * (float)m_worldScale);

            float DistanceToCamera = glm::distance(CameraPos, PatchCenter);

            int CoreLod = DistanceToLod(DistanceToCamera);

            PatchLod* pPatchLOD = m_map.get_a(LodMapX, LodMapZ);
            pPatchLOD->Core = CoreLod;
        }
    }
}


void LODManager::UpdateLodMapPass2(const glm::vec3& CameraPos) {
    int Step = m_patchSize / 2;

    for (int LodMapZ = 0 ; LodMapZ < m_numPatchesZ ; LodMapZ++) {
        for (int LodMapX = 0 ; LodMapX < m_numPatchesX ; LodMapX++) {
            int CoreLod = m_map.get(LodMapX, LodMapZ).Core;

            int IndexLeft   = LodMapX;
            int IndexRight  = LodMapX;
            int IndexTop    = LodMapZ;
            int IndexBottom = LodMapZ;

            if (LodMapX > 0) {
                IndexLeft--;

                if (m_map.get(IndexLeft, LodMapZ).Core > CoreLod) {
                    m_map[LodMapX][LodMapZ].Left = 1;
                } else {
                    m_map[LodMapX][LodMapZ].Left = 0;
                }
            }

            if (LodMapX < m_numPatchesX - 1) {
                IndexRight++;

                if (m_map.get(IndexRight, LodMapZ).Core > CoreLod) {
                    m_map[LodMapX][LodMapZ].Right = 1;
                } else {
                    m_map[LodMapX][LodMapZ].Right = 0;
                }
            }

            if (LodMapZ > 0) {
                IndexBottom--;

                if (m_map.get(LodMapX, IndexBottom).Core > CoreLod) {
                    m_map[LodMapX][LodMapZ].Bottom = 1;
                } else {
                    m_map[LodMapX][LodMapZ].Bottom = 0;
                }
            }

            if (LodMapZ < m_numPatchesZ - 1) {
                IndexTop++;

                if (m_map.get(LodMapX, IndexTop).Core > CoreLod) {
                    m_map[LodMapX][LodMapZ].Top = 1;
                } else {
                    m_map[LodMapX][LodMapZ].Top = 0;
                }
            }
        }
    }
}


void LODManager::PrintLodMap() {
    for (int LodMapZ = m_numPatchesZ - 1 ; LodMapZ >= 0 ; LodMapZ--) {
        printf("%d: ", LodMapZ);
        for (int LodMapX = 0 ; LodMapX < m_numPatchesX ; LodMapX++) {
            printf("%d ", m_map.get(LodMapX, LodMapZ).Core);
        }   
        printf("\n");
    }
}


int LODManager::DistanceToLod(float Distance) {
    int Lod = m_maxLOD;

    for (int i = 0 ; i <= m_maxLOD ; i++) {
        if (Distance < m_regions[i]) {
            Lod = i;
            break;
        }
    }

    return Lod;
}


const LODManager::PatchLod& LODManager::GetPatchLod(int PatchX, int PatchZ) const {
    return m_map.get(PatchX, PatchZ);
}


void LODManager::CalcLodRegions() {
    int Sum = 0;

    for (int i = 0 ; i <= m_maxLOD ; i++) {
        Sum += (i + 1);
    }

    printf("Sum %d\n", Sum);

    float X = Z_FAR / (float)Sum;

    int Temp = 0;

    for (int i = 0 ; i <= m_maxLOD ; i++) {
        int CurRange = (int)(X * (i + 1));
        m_regions[i] = Temp + CurRange;
        Temp += CurRange;
        printf("%d %d\n", i, m_regions[i]);
    }
}
