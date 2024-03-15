#include "ocean.h"

static float Phillips(const Vector2& k, const Vector2& w, float V, float A)
{
	float L = (V * V) / GRAV_ACCELERATION;	// largest possible wave for wind speed V
	float l = L / 1000.0f;					// supress waves smaller than this

	float kdotw = Vec2Dot(k, w);
	float k2 = Vec2Dot(k, k);			// squared length of wave vector k

	// k^6 because k must be normalized
	float P_h = A * (expf(-1.0f / (k2 * L * L))) / (k2 * k2 * k2) * (kdotw * kdotw);

	if (kdotw < 0.0f) {
		// wave is moving against wind direction w
		P_h *= 0.07f;
	}

	return P_h * expf(-k2 * l * l);
}

bool Ocean::Init() {
    std::mt19937 gen;
    std::normal_distribution<> gaussian(0.0, 1.0);
    GLint maxanisotropy = 1;

    glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxanisotropy);
    maxanisotropy = std::max(maxanisotropy, 2);

    // initial spectrum & frequencies
    Vector2 k;
    float L = PATCH_SIZE;
    glGenTextures(1, &spectrum);
    glGenTextures(1, &frequencies);
    glBindTexture(GL_TEXTURE_2D, spectrum);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, DISP_MAP_SIZE + 1, DISP_MAP_SIZE + 1);

    glBindTexture(GL_TEXTURE_2D, frequencies);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, DISP_MAP_SIZE + 1, DISP_MAP_SIZE + 1);

    // n, m should be be in [-N / 2, N / 2]
    int start = DISP_MAP_SIZE / 2;

    // NOTE: in order to be symmetric, this must be (N + 1) x (N + 1) in size
    Complex* h0data = new Complex[(DISP_MAP_SIZE + 1) * (DISP_MAP_SIZE + 1)];
    float* wdata = new float[(DISP_MAP_SIZE + 1) * (DISP_MAP_SIZE + 1)];
    {
        Vector2 w = WIND_DIRECTION;
        Vector2 wn;
        float V = WIND_SPEED;
        float A = AMPLITUDE_CONSTANT;

        Vec2Normalize(wn, w);

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
                wdata[index] = sqrtf(GRAV_ACCELERATION * Vec2Length(k));
            }
        }
    }

    return false;
}