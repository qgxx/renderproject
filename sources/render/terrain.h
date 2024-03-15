#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <math.h>

#include "shader.h"
#include "../core/qgearray.h"
#include "terrain_trianglelist.h"
#include "geomip_grid.h"

typedef struct Tile {
    unsigned int id;
    std::string type;
} Tile;

class Terrain {
public:
    Terrain(float WorldScale, const char* path);
    Terrain(float WorldScale, int PatchSize) {
        mWorldScale = WorldScale;
        mPatchSize = PatchSize;
        CreateMidpointDisplacement(513, 33, 1.0f, 0.0f, 256.0f);
        mGeoMipGrid.Create(513, 513, 33, this);
    }
    ~Terrain() = default;
    void Draw(Shader& shader, const glm::vec3 CameraPos);
    void destroy() {
        mHeightMap.destroy();
        // mTriangleList.destroy();
        mGeoMipGrid.Destroy();
    }
    float GetHeight(int x, int z) const { return mHeightMap[x][z]; }
    float GetWorldScale() const { return mWorldScale; }
    float GetTexScale() const { return mTexScale; }
    void setWorldScale(float scale) { mWorldScale = scale; }
    void setTexScale(float scale) { mTexScale = scale; }
    int getSize() const { return mTerrainSize; }
    void CreateMidpointDisplacement(int Size, float Roughness, float MinHeight, float MaxHeight);
    void CreateMidpointDisplacement(int Size, int PatchSize, float Roughness, float MinHeight, float MaxHeight);
    void setMinMAxHeight(float minH, float maxH) { mMinH = minH; mMaxH = maxH; }
    void loadTiles(const std::vector<std::pair<std::string, std::string>>& paths);
    void saveHeightMap(const char* path);
    glm::vec2 getCenterPos() { return mGeoMipGrid.getCenterPos(); }

private:
    float mWorldScale = 1.0f;
    float mTexScale = 1.0f;
    int mTerrainSize = 0;
    Array2d<float> mHeightMap;
    TriangleList mTriangleList;
    GeoMipGrid mGeoMipGrid;
    float mMinH, mMaxH;
    int mPatchSize = 0;
    std::vector<Tile> Tiles;

    void LoadHightMap(const char* path);
    void CreateMidpointDisplacementF32(float roughness);
    void diamondStep(int RectSize, float CurHeight);
    void squareStep(int RectSize, float CurHeight);
};

char* ReadBinaryFile(const char* path, size_t& size);
void WriteBinaryFile(const char* path, const void* data, size_t& size);
unsigned int TextureFromFile(const std::string& path);

#endif // !__TERRAIN_H__