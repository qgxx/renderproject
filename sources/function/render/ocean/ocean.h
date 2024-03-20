#ifndef __OCEAN_H__
#define __OCEAN_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <complex>
#include <random>
#include "../shader.h"
#include "mesh.h"
#include "core/qgemath.h"
#include "quadtree.h"

#define DISP_MAP_SIZE		512					// 1024 max
#define MESH_SIZE			256					// [64, 256] (or calculate index count for other levels)
#define GRAV_ACCELERATION	9.81f				// m/s^2
#define PATCH_SIZE			20.0f				// m
#define FURTHEST_COVER		8					// full ocean size = PATCH_SIZE * (1 << FURTHEST_COVER)
#define MAX_COVERAGE		64.0f				// pixel limit for a distant patch to be rendered
#define WIND_DIRECTION		{ -0.4f, -0.9f }
#define WIND_SPEED			6.5f				// m/s
#define AMPLITUDE_CONSTANT	(0.45f * 1e-3f)		// for the (modified) Phillips spectrum

static const int IndexCounts[] = {
	0,
	0,
	0,
	0,
	0,
	0,
	961920,		// 64x64
	3705084,	// 128x128
	14500728	// 256x256
};

class Ocean {
public:
    Ocean() {}
    ~Ocean() {}
    bool Init();
    void Render(glm::mat4 world, glm::mat4 viewproj, glm::mat4 proj, glm::vec3 eye, double Elapsed);
    unsigned int getDisplacementID() { return displacement; }

private:
    unsigned int init_spectrum, frequencies, updated[2], tempdata, displacement, gradients;
    unsigned int perlin_noise, envmap;
    Shader* spectrumShader;
    Shader* fftShader;
    Shader* displacementShader;
    Shader* gradientShader;
    Shader* oceanShader;
    oMesh* oceanMesh;
    QuadTree tree;

    uint32_t numlods = 0;

    void FourierTransform(GLuint spectrum);
    void GenerateLODLevels(OceanAttribute** subsettable, GLuint* numsubsets, uint32_t* idata);
    GLuint GenerateBoundaryMesh(int deg_left, int deg_top, int deg_right, int deg_bottom, int levelsize, uint32_t* idata);
    unsigned int TextureFromFile(const char* path);
    unsigned int loadCubemap(std::string directory);
};

static GLuint CalcSubsetIndex(int level, int dL, int dR, int dB, int dT)
{
	// returns the subset index of the given LOD levels
	return 2 * (level * 3 * 3 * 3 * 3 + dL * 3 * 3 * 3 + dR * 3 * 3 + dB * 3 + dT);
}

#endif // !__OCEAN_H__