#include "terrain.h"

#define QGERROR(msg)\
    do {\
        fprintf(stderr, "%s:%d %s", __FILE__, __LINE__, msg);\
    } while(0)

Terrain::Terrain(float WorldScale, const char* path) {
    mWorldScale = WorldScale;
    LoadHightMap(path);
}

void Draw(Shader& shader) {
    shader.use();
}

void Terrain::LoadHightMap(const char* path) {
    size_t size = 0;
    unsigned char* p = (unsigned char*)ReadBinaryFile(path, size);

    if (size % sizeof(float) != 0) {
        printf("%s:%d - '%s' does not contain an whole number of floats (size %d)\n", __FILE__, __LINE__, path, size);
        exit(0);
    }
    mTerrainSize = (int)sqrtf((float)size / (float)sizeof(float));

    if ((mTerrainSize * mTerrainSize) != (size / sizeof(float))) {
        printf("%s:%d - '%s' does not contain a square height map - size %d\n", __FILE__, __LINE__, path, size);
        exit(0);
    }

    mHeightMap.InitArray2D(mTerrainSize, mTerrainSize, (float*)p);
}

char* ReadBinaryFile(const char* path, size_t& size) {
    FILE* f = NULL;
    errno_t err = fopen_s(&f, path, "rb");
    if (!f) {
        char buf[256] { 0 };
        strerror_s(buf, sizeof(buf), err);
        QGERROR(buf);
        exit(0);
    }

    struct stat stat_buf;
    int error = stat(path, &stat_buf);
    if (error) {
        char buf[256] { 0 };
        strerror_s(buf, sizeof(buf), err);
        QGERROR(buf);
        return NULL;
    }
    size = stat_buf.st_size;

    char* p = (char*)malloc(size);
    assert(p);

    size_t bytes_read = fread(p, 1, size, f);
    if (bytes_read != size) {
        char buf[256] { 0 };
        strerror_s(buf, sizeof(buf), err);
        QGERROR(buf);
        exit(0);
    }
    
    fclose(f);
    return p;
}