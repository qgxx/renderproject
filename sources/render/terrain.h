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
#include "ogldev_array_2d.h"
#include "terrain_trianglelist.h"

class Terrain {
public:
    Terrain(float WorldScale, const char* path);
    ~Terrain() = default;
    void Draw(Shader& shader) { mTriangleList.Draw(shader); }
    float GetHeight(int x, int z) const { return mHeightMap.Get(x, z); }
    float GetWorldScale() const { return mWorldScale; }

private:
    float mWorldScale = 1.0f;
    int mTerrainSize = 0;
    Array2D<float> mHeightMap;
    TriangleList mTriangleList;

    void LoadHightMap(const char* path);
};

char* ReadBinaryFile(const char* path, size_t& size);

#endif // !__TERRAIN_H__