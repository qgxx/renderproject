#include "terrain.h"
#include "../core/qgemath.h"
#include <stb/stb_image.h>

#define QGERROR(msg)\
    do {\
        fprintf(stderr, "%s:%d %s", __FILE__, __LINE__, msg);\
    } while(0)

Terrain::Terrain(float WorldScale, const char* path) {
    mWorldScale = WorldScale;
    LoadHightMap(path);
    mTriangleList.CreateTriangleList(mTerrainSize, mTerrainSize, this);
}

void Terrain::Draw(Shader& shader) {
    shader.use();
    shader.setFloat("gMinHeight", mMinH);
    shader.setFloat("gMaxHeight", mMaxH);
    string name = "gTextureHeight";
    for (int i = 0; i < Tiles.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        shader.setInt((name + to_string(i)).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, Tiles[i].id);
    }
    mTriangleList.Draw(shader);
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

    mHeightMap.set(mTerrainSize, mTerrainSize, (float*)p);
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

void Terrain::CreateMidpointDisplacement(int Size, float Roughness, float MinHeight, float MaxHeight) {
    if (Roughness < 0.0f) exit(0);
    mTerrainSize = Size;
    setMinMAxHeight(MinHeight, MaxHeight);
    mHeightMap.set_all(Size, Size, 0.0f);
    CreateMidpointDisplacementF32(Roughness);
    mHeightMap.normalize(MinHeight, MaxHeight);
    mTriangleList.CreateTriangleList(mTerrainSize, mTerrainSize, this);
}

void Terrain::CreateMidpointDisplacementF32(float roughness) {
    int size = next_power_of2(mTerrainSize);
    float CurHeight = (float)size / 2.0f;
    float HeightReduce = pow(2.0f, -roughness);

    while (size > 0) {
        diamondStep(size, CurHeight);
        squareStep(size, CurHeight);

        size >>= 1;
        CurHeight *= HeightReduce;
    }
}

void Terrain::diamondStep(int RectSize, float CurHeight) {
    int HalfRectSize = RectSize / 2;

    for (int y = 0 ; y < mTerrainSize; y += RectSize) {
        for (int x = 0 ; x < mTerrainSize; x += RectSize) {
            int next_x = (x + RectSize) % mTerrainSize;
            int next_y = (y + RectSize) % mTerrainSize;

            if (next_x < x) {
                next_x = mTerrainSize - 1;
            }

            if (next_y < y) {
                next_y = mTerrainSize - 1;
            }

            float TopLeft     = mHeightMap.get(x, y);
            float TopRight    = mHeightMap.get(next_x, y);
            float BottomLeft  = mHeightMap.get(x, next_y);
            float BottomRight = mHeightMap.get(next_x, next_y);

            int mid_x = (x + HalfRectSize) % mTerrainSize;
            int mid_y = (y + HalfRectSize) % mTerrainSize;

            float RandValue = RandomFloatRange(CurHeight, -CurHeight);
            float MidPoint = (TopLeft + TopRight + BottomLeft + BottomRight) / 4.0f;
            mHeightMap.set(mid_x, mid_y, MidPoint + RandValue);
        }
    }
}


void Terrain::squareStep(int RectSize, float CurHeight) {
    int HalfRectSize = RectSize / 2;

    for (int y = 0 ; y < mTerrainSize ; y += RectSize) {
        for (int x = 0 ; x < mTerrainSize ; x += RectSize) {
            int next_x = (x + RectSize) % mTerrainSize;
            int next_y = (y + RectSize) % mTerrainSize;

            if (next_x < x) {
                next_x = mTerrainSize - 1;
            }

            if (next_y < y) {
                next_y = mTerrainSize - 1;
            }

            int mid_x = (x + HalfRectSize) % mTerrainSize;
            int mid_y = (y + HalfRectSize) % mTerrainSize;
              
            int prev_mid_x = (x - HalfRectSize + mTerrainSize) % mTerrainSize;
            int prev_mid_y = (y - HalfRectSize + mTerrainSize) % mTerrainSize;

            float CurTopLeft  = mHeightMap.get(x, y);
            float CurTopRight = mHeightMap.get(next_x, y);
            float CurCenter   = mHeightMap.get(mid_x, mid_y);
            float PrevYCenter = mHeightMap.get(mid_x, prev_mid_y);
            float CurBotLeft  = mHeightMap.get(x, next_y);
            float PrevXCenter = mHeightMap.get(prev_mid_x, mid_y);

            float CurLeftMid = (CurTopLeft + CurCenter + CurBotLeft + PrevXCenter) / 4.0f + RandomFloatRange(-CurHeight, CurHeight);
            float CurTopMid  = (CurTopLeft + CurCenter + CurTopRight + PrevYCenter) / 4.0f + RandomFloatRange(-CurHeight, CurHeight);

            mHeightMap.set(mid_x, y, CurTopMid);
            mHeightMap.set(x, mid_y, CurLeftMid);
        }
    }
}

void Terrain::loadTiles(const vector<pair<string, string>>& paths) {
    for (int i = 0; i < paths.size(); i++) 
        Tiles.push_back({TextureFromFile(paths[i].first), paths[i].second});
}

unsigned int TextureFromFile(const string& path) {
    unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
    return textureID;
}