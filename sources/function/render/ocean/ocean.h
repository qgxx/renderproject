#ifndef __OCEAN_H__
#define __OCEAN_H__

#include <glm/glm.hpp>
#include <complex>
#include <random>
#include "../shader.h"
#include "mesh.h"
#include "core/qgemath.h"

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
    Ocean() {
        spectrumShader = new Shader("..\\asserts\\shaders\\spectrum.comp");
        fftShader = new Shader("..\\asserts\\shaders\\fft.comp");
        displacementShader = new Shader("..\\asserts\\shaders\\displacement.comp");
    }
    ~Ocean() {}
    bool Init();
    void Render();
    unsigned int getDisplacementID() { return displacement; }

private:
    unsigned int init_spectrum, frequencies, updated[2], tempdata, displacement, gradients;
    Shader* spectrumShader;
    Shader* fftShader;
    Shader* displacementShader;
    oMesh* oceanMesh;

    uint32_t numlods = 0;

    void FourierTransform(GLuint spectrum) {
        fftShader->use();
        fftShader->setInt("readbuff", 0);
	    fftShader->setInt("writebuff", 1);

        // horizontal pass
        glBindImageTexture(0, spectrum, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RG32F);
        glBindImageTexture(1, tempdata, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG32F);
        glDispatchCompute(DISP_MAP_SIZE, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // vertical pass
        glBindImageTexture(0, tempdata, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RG32F);
        glBindImageTexture(1, spectrum, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG32F);
        glDispatchCompute(DISP_MAP_SIZE, 1, 1);
    }
};

#endif // !__OCEAN_H__