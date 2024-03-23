#include "ocean.h"
#include <stb/stb_image.h>

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
	// Complex* h0data = new Complex[(DISP_MAP_SIZE + 1) * (DISP_MAP_SIZE + 1)];
	std::complex<float>* h0data = new std::complex<float>[(DISP_MAP_SIZE + 1) * (DISP_MAP_SIZE + 1)];
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

				h0data[index] = std::complex<float>((float)(sqrt_P_h * gaussian(gen) * ONE_OVER_SQRT_2), 
													(float)(sqrt_P_h * gaussian(gen) * ONE_OVER_SQRT_2));

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
	if (!GLCreateMesh((MESH_SIZE + 1) * (MESH_SIZE + 1), IndexCounts[numlods], OMESH_32BIT, decl, &oceanMesh)) return false;

	glm::vec3* vdata = nullptr;
	uint32_t* idata = nullptr;
	OceanAttribute* subsettable = nullptr;
	GLuint numSubsets = 0;
	if (!oceanMesh->LockVertexBuffer(0, 0, GLLOCK_DISCARD, (void**)&vdata)) return false;
	if (!oceanMesh->LockIndexBuffer(0, 0, GLLOCK_DISCARD, (void**)&idata)) {
		fprintf(stderr, "Create index-buffer error!");
		return false;
	}
	{
		// vertex data
		for (int z = 0; z <= MESH_SIZE; z++) {
			for (int x = 0; x <= MESH_SIZE; x++) {
				int index = z * (MESH_SIZE + 1) + x;
				vdata[index].x = (float)x;
				vdata[index].y = (float)z;
				vdata[index].z = 0.0f;
			}
		}

		// index data
		GenerateLODLevels(&subsettable, &numSubsets, idata);
	}
	oceanMesh->UnlockIndexBuffer();
	oceanMesh->UnlockVertexBuffer();
	oceanMesh->SetAttributeTable(subsettable, numSubsets);
	delete[] subsettable;

	// Shader
	spectrumShader = new Shader("..\\asserts\\shaders\\spectrum.comp");
    spectrumShader->use();
    spectrumShader->setInt("tilde_h0", 0);
    spectrumShader->setInt("frequencies", 1);
    spectrumShader->setInt("tilde_h", 2);
    spectrumShader->setInt("tilde_D", 3);

    fftShader = new Shader("..\\asserts\\shaders\\fft.comp");
	fftShader->use();
	fftShader->setInt("readbuff", 0);
	fftShader->setInt("writebuff", 1);

    displacementShader = new Shader("..\\asserts\\shaders\\displacement.comp");
    displacementShader->use();
    displacementShader->setInt("heightmap", 0);
	displacementShader->setInt("choppyfield", 1);
	displacementShader->setInt("displacement", 2);

	gradientShader = new Shader("..\\asserts\\shaders\\gradient.comp");
	gradientShader->use();
	gradientShader->setInt("displacement", 0);
	gradientShader->setInt("gradients", 1);

	oceanShader = new Shader("..\\asserts\\shaders\\ocean.vs", "..\\asserts\\shaders\\ocean.fs");
	oceanShader->use();
	oceanShader->setInt("displacement", 0);
	oceanShader->setInt("perlin", 1);
	oceanShader->setInt("envmap", 2);
	oceanShader->setInt("gradients", 3);

	// other texture
	perlin_noise = TextureFromFile("..\\asserts\\images\\perlin_noise.png");
	glBindTexture(GL_TEXTURE_2D, perlin_noise);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxanisotropy / 2);
	glBindTexture(GL_TEXTURE_2D, 0);
	envmap = loadCubemap(std::string("..\\asserts\\images\\ocean_env"));

	// quadtree
	float ocean_extent = PATCH_SIZE * (1 << FURTHEST_COVER);
	glm::vec2 ocean_start(-0.5f * ocean_extent, -0.5f * ocean_extent);
	tree.Initialize(ocean_start, ocean_extent, (int)numlods, MESH_SIZE, PATCH_SIZE, MAX_COVERAGE, (float)(1080 * 510));

	return true;
}

void Ocean::Render(glm::mat4 world, glm::mat4 proj, Camera& camera, double Elapsed) {
	static float time = 0.0f;

    spectrumShader->use();
    spectrumShader->setFloat("time", time);
    glBindImageTexture(0, init_spectrum, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RG32F);
    glBindImageTexture(1, frequencies, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);
    glBindImageTexture(2, updated[0], 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG32F);
    glBindImageTexture(3, updated[1], 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG32F);
    glDispatchCompute(DISP_MAP_SIZE / 16, DISP_MAP_SIZE / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// transform spectra to time domain
    FourierTransform(updated[0]);
    FourierTransform(updated[1]);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// calculate displacement map
    displacementShader->use();
	glBindImageTexture(0, updated[0], 0, GL_TRUE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, updated[1], 0, GL_TRUE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(2, displacement, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glDispatchCompute(DISP_MAP_SIZE / 16, DISP_MAP_SIZE / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// calculate normal & folding map
	gradientShader->use();
	glBindImageTexture(0, displacement, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, gradients, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA16F);
    glDispatchCompute(DISP_MAP_SIZE / 16, DISP_MAP_SIZE / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glBindTexture(GL_TEXTURE_2D, gradients);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// ocean render
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 viewproj = proj * view;
	glm::vec3 eye = camera.getPos();
	// world = glm::scale(world, glm::vec3(5.0f, 5.0f, 5.0f));
	// build quadtree
	tree.Rebuild(viewproj, proj, eye);
	
	glm::mat4 flipYZ(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 local_traf = glm::mat4(1.0f);
	glm::vec4 uvparams(0.0f, 0.0f, 0.0f, 0.0f);
	glm::vec2 perlin_offset(0.0f, 0.0f);
	glm::vec2 w = WIND_DIRECTION;
	int pattern[4];
	GLuint subset = 0;
	uvparams.x = 1.0f / PATCH_SIZE;
	uvparams.y = 0.5f / DISP_MAP_SIZE;
	perlin_offset.x = -w.x * time * 0.06f;
	perlin_offset.y = -w.y * time * 0.06f;
	oceanShader->use();
	oceanShader->setMat4("matViewProj", viewproj);
	oceanShader->setVec2("perlinOffset", perlin_offset);
	oceanShader->setVec3("eyePos", eye);
	oceanShader->setVec3("oceanColor", glm::vec3(0.1812f, 0.4678f, 0.5520f));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, displacement);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, perlin_noise);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envmap);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gradients);

	#if 0
	float levelsize = (float)(MESH_SIZE >> 0);
	float ocean_extent = PATCH_SIZE * (1 << FURTHEST_COVER);
	glm::vec2 ocean_start(-0.5f * ocean_extent, -0.5f * ocean_extent);
	float scale = ocean_extent / levelsize;
	local_traf = glm::scale(local_traf, glm::vec3(scale, scale, 0.0f));
	world = glm::translate(world, glm::vec3(ocean_start[0], 0.0f, ocean_start[1]));
	world = world * flipYZ;
	uvparams.z = ocean_start[0] / PATCH_SIZE;
	uvparams.w = ocean_start[1] / PATCH_SIZE;
	oceanShader->setMat4("matLocal", local_traf);
	oceanShader->setMat4("matWorld", world);
	oceanShader->setVec4("uvParams", uvparams);
	oceanMesh->Draw();
	#endif

	// QuadTree LOD not work...
	#if 1
	tree.Traverse([&](const QuadTree::Node& node) {
		float levelsize = (float)(MESH_SIZE >> node.lod);
		float scale = node.length / levelsize;
		local_traf = glm::scale(local_traf, glm::vec3(scale, scale, 0.0f));
		world = glm::translate(world, glm::vec3(node.start[0], 0.0f, node.start[1]));
		world = glm::translate(world, glm::vec3(2000.0f, 0.0f, 2000.0f));
		world = world * flipYZ;

		uvparams.z = node.start[0] / PATCH_SIZE;
		uvparams.w = node.start[1] / PATCH_SIZE;

		oceanShader->use();
		oceanShader->setMat4("matLocal", local_traf);
		oceanShader->setMat4("matWorld", world);
		oceanShader->setVec4("uvParams", uvparams);

		tree.FindSubsetPattern(pattern, node);
		subset = CalcSubsetIndex(node.lod, pattern[0], pattern[1], pattern[2], pattern[3]);
		if (subset < oceanMesh->GetNumSubsets() - 1) {
			oceanMesh->DrawSubset(subset);
			oceanMesh->DrawSubset(subset + 1);
		}
	});
	#endif
	glActiveTexture(GL_TEXTURE0);
	time += Elapsed;
}

void Ocean::FourierTransform(GLuint spectrum) {
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

void Ocean::GenerateLODLevels(OceanAttribute** subsettable, GLuint* numsubsets, uint32_t* idata) {
#define CALC_INNER_INDEX(x, z) \
	((top + (z)) * (MESH_SIZE + 1) + left + (x))
// END

	assert(subsettable);
	assert(numsubsets);

	*numsubsets = (numlods - 2) * 3 * 3 * 3 * 3 * 2;
	*subsettable = new OceanAttribute[*numsubsets];

	int currsubset = 0;
	GLuint indexoffset = 0;
	GLuint numwritten = 0;
	OceanAttribute* subset = 0;

	for (uint32_t level = 0; level < numlods - 2; ++level) {
		int levelsize = MESH_SIZE >> level;
		int mindegree = levelsize >> 3;

		for (int left_degree = levelsize; left_degree > mindegree; left_degree >>= 1) {
			for (int right_degree = levelsize; right_degree > mindegree; right_degree >>= 1) {
				for (int bottom_degree = levelsize; bottom_degree > mindegree; bottom_degree >>= 1) {
					for (int top_degree = levelsize; top_degree > mindegree; top_degree >>= 1) {
						int right	= ((right_degree == levelsize) ? levelsize : levelsize - 1);
						int left	= ((left_degree == levelsize) ? 0 : 1);
						int bottom	= ((bottom_degree == levelsize) ? levelsize : levelsize - 1);
						int top		= ((top_degree == levelsize) ? 0 : 1);

						// generate inner mesh (triangle strip)
						int width = right - left;
						int height = bottom - top;

						numwritten = 0;

						for (int z = 0; z < height; ++z) {
							if ((z & 1) == 1) {
								idata[numwritten++] = CALC_INNER_INDEX(0, z);
								idata[numwritten++] = CALC_INNER_INDEX(0, z + 1);

								for (int x = 0; x < width; ++x) {
									idata[numwritten++] = CALC_INNER_INDEX(x + 1, z);
									idata[numwritten++] = CALC_INNER_INDEX(x + 1, z + 1);
								}

								idata[numwritten++] = UINT32_MAX;
							} else {
								idata[numwritten++] = CALC_INNER_INDEX(width, z + 1);
								idata[numwritten++] = CALC_INNER_INDEX(width, z);

								for (int x = width - 1; x >= 0; --x) {
									idata[numwritten++] = CALC_INNER_INDEX(x, z + 1);
									idata[numwritten++] = CALC_INNER_INDEX(x, z);
								}

								idata[numwritten++] = UINT32_MAX;
							}
						}

						// add inner subset
						subset = ((*subsettable) + currsubset);

						subset->attributeId		= currsubset;
						subset->enabled			= (numwritten > 0);
						subset->indexCount		= numwritten;
						subset->indexStart		= indexoffset;
						subset->primitiveType	= GL_TRIANGLE_STRIP;
						subset->vertexCount		= 0;
						subset->vertexStart		= 0;

						indexoffset += numwritten;
						idata += numwritten;

						++currsubset;

						// generate boundary mesh (triangle list)
						numwritten = GenerateBoundaryMesh(left_degree, top_degree, right_degree, bottom_degree, levelsize, idata);

						// add boundary subset
						subset = ((*subsettable) + currsubset);

						subset->attributeId		= currsubset;
						subset->enabled			= (numwritten > 0);
						subset->indexCount		= numwritten;
						subset->indexStart		= indexoffset;
						subset->primitiveType	= GL_TRIANGLES;
						subset->vertexCount		= 0;
						subset->vertexStart		= 0;

						indexoffset += numwritten;
						idata += numwritten;

						++currsubset;
					}
				}
			}
		}
	}
}

GLuint Ocean::GenerateBoundaryMesh(int deg_left, int deg_top, int deg_right, int deg_bottom, int levelsize, uint32_t* idata) {
#define CALC_BOUNDARY_INDEX(x, z) \
	((z) * (MESH_SIZE + 1) + (x))
// END

	GLuint numwritten = 0;

	// top edge
	if (deg_top < levelsize) {
		int t_step = levelsize / deg_top;

		for (int i = 0; i < levelsize; i += t_step) {
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i, 0);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i + t_step / 2, 1);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i + t_step, 0);

			for (int j = 0; j < t_step / 2; ++j) {
				if (i == 0 && j == 0 && deg_left < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(i, 0);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j, 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j + 1, 1);
			}

			for (int j = t_step / 2; j < t_step; ++j) {
				if (i == levelsize - t_step && j == t_step - 1 && deg_right < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + t_step, 0);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j, 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j + 1, 1);
			}
		}
	}

	// left edge
	if (deg_left < levelsize) {
		int l_step = levelsize / deg_left;

		for (int i = 0; i < levelsize; i += l_step) {
			idata[numwritten++] = CALC_BOUNDARY_INDEX(0, i);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(0, i + l_step);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(1, i + l_step / 2);

			for (int j = 0; j < l_step / 2; ++j) {
				if (i == 0 && j == 0 && deg_top < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(0, i);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(1, i + j + 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(1, i + j);
			}

			for (int j = l_step / 2; j < l_step; ++j) {
				if (i == levelsize - l_step && j == l_step - 1 && deg_bottom < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(0, i + l_step);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(1, i + j + 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(1, i + j);
			}
		}
	}

	// right edge
	if (deg_right < levelsize) {
		int r_step = levelsize / deg_right;

		for (int i = 0; i < levelsize; i += r_step) {
			idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize, i);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize - 1, i + r_step / 2);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize, i + r_step);

			for (int j = 0; j < r_step / 2; ++j) {
				if (i == 0 && j == 0 && deg_top < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize, i);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize - 1, i + j);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize - 1, i + j + 1);
			}

			for (int j = r_step / 2; j < r_step; ++j) {
				if (i == levelsize - r_step && j == r_step - 1 && deg_bottom < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize, i + r_step);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize - 1, i + j);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize - 1, i + j + 1);
			}
		}
	}

	// bottom edge
	if (deg_bottom < levelsize) {
		int b_step = levelsize / deg_bottom;

		for (int i = 0; i < levelsize; i += b_step) {
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i, levelsize);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i + b_step, levelsize);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i + b_step / 2, levelsize - 1);

			for (int j = 0; j < b_step / 2; ++j) {
				if (i == 0 && j == 0 && deg_left < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(i, levelsize);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j + 1, levelsize - 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j, levelsize - 1);
			}

			for (int j = b_step / 2; j < b_step; ++j) {
				if (i == levelsize - b_step && j == b_step - 1 && deg_right < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + b_step, levelsize);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j + 1, levelsize - 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j, levelsize - 1);
			}
		}
	}

	return numwritten;
}

unsigned int Ocean::TextureFromFile(const char* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
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

unsigned int Ocean::loadCubemap(std::string directory) {
#define LOAD_FACE(path, i)\
    do {\
        int width, height, nrChannels;\
        unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);\
        if (data) {\
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);\
            stbi_image_free(data);\
        }\
        else {\
            std::cout << "Cubemap texture failed to load at path: " << path << std::endl;\
            stbi_image_free(data);\
        }\
    } while(0)
    unsigned int TexID;
    glGenTextures(1, &TexID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, TexID);
    LOAD_FACE((directory + "\\right.png").c_str(), 0);
    LOAD_FACE((directory + "\\left.png").c_str(), 1);
    LOAD_FACE((directory + "\\top.png").c_str(), 2);
    LOAD_FACE((directory + "\\bottom.png").c_str(), 3);
    LOAD_FACE((directory + "\\front.png").c_str(), 4);
    LOAD_FACE((directory + "\\back.png").c_str(), 5);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return TexID;
}