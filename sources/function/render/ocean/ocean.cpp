#include "ocean.h"

extern GLint maxanisotropy;

static float Phillips(const glm::vec2& k, const glm::vec2& w, float V, float A) {
	float L = (V * V) / 9.81f;	// largest possible wave for wind speed V
	float l = L / 1000.0f;					// supress waves smaller than this

	float kdotw = glm::dot(k, w);
	float k2 = glm::dot(k, k);			// squared length of wave vector k

	// k^6 because k must be normalized
	float P_h = A * (expf(-1.0f / (k2 * L * L))) / (k2 * k2 * k2) * (kdotw * kdotw);

	if (kdotw < 0.0f) {
		// wave is moving against wind direction w
		P_h *= 0.07f;
	}

	return P_h * expf(-k2 * l * l);
}

bool Ocean::Init() {
    static std::mt19937 gen;
    static std::normal_distribution<> gaussian(0.0, 1.0);

	// generate initial spectrum and frequencies
	glm::vec2 k;
	float L = PATCH_SIZE;

    glGenTextures(1, &init_spectrum);
	glBindTexture(GL_TEXTURE_2D, init_spectrum);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, DISP_MAP_SIZE + 1, DISP_MAP_SIZE + 1);

    glGenTextures(1, &frequencies);
	glBindTexture(GL_TEXTURE_2D, frequencies);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, DISP_MAP_SIZE + 1, DISP_MAP_SIZE + 1);

	// n, m should be be in [-N / 2, N / 2]
	int start = DISP_MAP_SIZE / 2;

	// NOTE: in order to be symmetric, this must be (N + 1) x (N + 1) in size
	Complex* h0data = new Complex[(DISP_MAP_SIZE + 1) * (DISP_MAP_SIZE + 1)];
	float* wdata = new float[(DISP_MAP_SIZE + 1) * (DISP_MAP_SIZE + 1)];
	{
		glm::vec2 w = WIND_DIRECTION;
		glm::vec2 wn;
		float V = WIND_SPEED;
		float A = AMPLITUDE_CONSTANT;

		wn = glm::normalize(w);

		for (int m = 0; m <= DISP_MAP_SIZE; ++m) {
            
			k.y = (TWO_PI * (start - m)) / L;

			for (int n = 0; n <= DISP_MAP_SIZE; ++n) {
				k.x = (TWO_PI * (start - n)) / L;
				
				int index = m * (DISP_MAP_SIZE + 1) + n;
				float sqrt_P_h = 0;

				if (k.x != 0.0f || k.y != 0.0f)
					sqrt_P_h = sqrtf(Phillips(k, wn, V, A));

				h0data[index].a = (float)(sqrt_P_h * gaussian(gen) * ONE_OVER_SQRT_2);
				h0data[index].b = (float)(sqrt_P_h * gaussian(gen) * ONE_OVER_SQRT_2);

				// dispersion relation \omega^2(k) = gk
				wdata[index] = sqrtf(GRAV_ACCELERATION * glm::length(k));
			}
		}
	}
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, DISP_MAP_SIZE + 1, DISP_MAP_SIZE + 1, GL_RED, GL_FLOAT, wdata);

	glBindTexture(GL_TEXTURE_2D, init_spectrum);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, DISP_MAP_SIZE + 1, DISP_MAP_SIZE + 1, GL_RG, GL_FLOAT, h0data);

    delete[] wdata;
    delete[] h0data;

	// create other spectrum textures
	glGenTextures(2, updated);
	glBindTexture(GL_TEXTURE_2D, updated[0]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, DISP_MAP_SIZE, DISP_MAP_SIZE);

	glBindTexture(GL_TEXTURE_2D, updated[1]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, DISP_MAP_SIZE, DISP_MAP_SIZE);

	glGenTextures(1, &tempdata);
	glBindTexture(GL_TEXTURE_2D, tempdata);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, DISP_MAP_SIZE, DISP_MAP_SIZE);

	// create displacement map
	glGenTextures(1, &displacement);
	glBindTexture(GL_TEXTURE_2D, displacement);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, DISP_MAP_SIZE, DISP_MAP_SIZE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// create gradient & folding map
	glGenTextures(1, &gradients);
	glBindTexture(GL_TEXTURE_2D, gradients);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, DISP_MAP_SIZE, DISP_MAP_SIZE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxanisotropy / 2);

    glBindTexture(GL_TEXTURE_2D, 0);

	// create mesh and LOD levels (could use tess shader in the future)
	OceanVertexElement decl[] = {
		{ 0, 0, GLDECLTYPE_FLOAT3, GLDECLUSAGE_POSITION, 0 },
		{ 0xff, 0, 0, 0, 0 }
	};
	numlods = Log2OfPow2(MESH_SIZE);
	if (!GLCreateMesh((MESH_SIZE + 1) * (MESH_SIZE + 1), IndexCounts[numlods], OMESH_32BIT, &oceanMesh)) return false;

	glm::vec3* vdata = nullptr;
	uint32_t* idata = nullptr;
	OceanAttribute* subsettable = nullptr;
	GLuint numSubsets = 0;

	{
		// vertex data
		vdata = new glm::vec3[(MESH_SIZE + 1) * (MESH_SIZE + 1)];
		for (int z = 0; z <= MESH_SIZE; ++z) {
			for (int x = 0; x <= MESH_SIZE; ++x) {
				int index = z * (MESH_SIZE + 1) + x;

				vdata[index].x = (float)x;
				vdata[index].y = (float)z;
				vdata[index].z = 0.0f;
			}
		}
	}

	return true;
}

void Ocean::Render() {
    spectrumShader->use();
    spectrumShader->setInt("tilde_h0", 0);
    spectrumShader->setInt("frequencies", 1);
    spectrumShader->setInt("tilde_h", 2);
    spectrumShader->setInt("tilde_D", 3);
    spectrumShader->setFloat("time", 0.06f);
    glBindImageTexture(0, init_spectrum, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RG32F);
    glBindImageTexture(1, frequencies, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);

    glBindImageTexture(2, updated[0], 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG32F);
    glBindImageTexture(3, updated[1], 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG32F);

    glDispatchCompute(DISP_MAP_SIZE / 16, DISP_MAP_SIZE / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    FourierTransform(updated[0]);
    FourierTransform(updated[1]);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// calculate displacement map
    displacementShader->use();
    displacementShader->setInt("heightmap", 0);
	displacementShader->setInt("choppyfield", 1);
	displacementShader->setInt("displacement", 2);
	glBindImageTexture(0, updated[0], 0, GL_TRUE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, updated[1], 0, GL_TRUE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(2, displacement, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glDispatchCompute(DISP_MAP_SIZE / 16, DISP_MAP_SIZE / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}