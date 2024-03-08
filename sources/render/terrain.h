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

using namespace std;

typedef struct Tile {
    unsigned int id;
    string type;
} Tile;

class Terrain {
public:
    Terrain(float WorldScale, const char* path);
    ~Terrain() = default;
    void Draw(Shader& shader);
    void destroy() {
        mHeightMap.destroy();
        mTriangleList.destroy();
    }
    float GetHeight(int x, int z) const { return mHeightMap[x][z]; }
    float GetWorldScale() const { return mWorldScale; }
    float GetTexScale() const { return mTexScale; }
    void setWorldScale(float scale) { mWorldScale = scale; }
    void setTexScale(float scale) { mTexScale = scale; }
    int getSize() const { return mTerrainSize; }
    void CreateMidpointDisplacement(int Size, float Roughness, float MinHeight, float MaxHeight);
    void setMinMAxHeight(float minH, float maxH) { mMinH = minH; mMaxH = maxH; }
    void loadTiles(const vector<pair<string, string>>& paths);

private:
    float mWorldScale = 1.0f;
    float mTexScale = 1.0f;
    int mTerrainSize = 0;
    Array2d<float> mHeightMap;
    TriangleList mTriangleList;
    float mMinH, mMaxH;
    vector<Tile> Tiles;

    void LoadHightMap(const char* path);
    void CreateMidpointDisplacementF32(float roughness);
    void diamondStep(int RectSize, float CurHeight);
    void squareStep(int RectSize, float CurHeight);
};

char* ReadBinaryFile(const char* path, size_t& size);
unsigned int TextureFromFile(const string& path);

#endif // !__TERRAIN_H__