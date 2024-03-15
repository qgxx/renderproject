#ifndef __OCEAN_H__
#define __OCEAN_H__

#include <glad/gl.h>
#include <OpenGL/glextensions.h>
#include <glm/glm.hpp>
#include <cmath>
#include <core/qgemath.h>
#include <random>

#define DISP_MAP_SIZE		512					// 1024 max
#define MESH_SIZE			256					// [64, 256] (or calculate index count for other levels)
#define GRAV_ACCELERATION	9.81f				// m/s^2
#define PATCH_SIZE			20.0f				// m
#define FURTHEST_COVER		8					// full ocean size = PATCH_SIZE * (1 << FURTHEST_COVER)
#define MAX_COVERAGE		64.0f				// pixel limit for a distant patch to be rendered
#define WIND_DIRECTION		{ -0.4f, -0.9f }
#define WIND_SPEED			6.5f				// m/s
#define AMPLITUDE_CONSTANT	(0.45f * 1e-3f)		// for the (modified) Phillips spectrum

class Ocean {
public:
    bool Init();

private:
    uint32_t width;
    uint32_t height;

    GLuint spectrum;
    GLuint frequencies;
};

#endif // !__OCEAN_H__