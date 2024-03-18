#ifndef __QGE_MATH_H__
#define __QGE_MATH_H__

#include <iostream>
#include <stdlib.h>
#include <glm/glm.hpp>

#define powi(base,exp) (int)powf((float)(base), (float)(exp))

static const float PI = 3.141592f;
static const float TWO_PI = 6.283185f;
static const float HALF_PI = 1.570796f;
static const float ONE_OVER_SQRT_2 = 0.7071067f;
static const float ONE_OVER_SQRT_TWO_PI = 0.39894228f;

inline bool isPowerOf2(int x) {
    return (x & (x - 1));
}

inline int next_power_of2(int x) {
    if (x == 1) return 2;
    int ret = 1;
    while (ret < x) ret <<= 1;
    return ret;
}

inline float RandomFloat() {
    float Max = RAND_MAX;
    return ((float)rand() / Max);
}

inline float RandomFloatRange(float Start, float End) {
    if (End == Start) {
        printf("Invalid random range: (%f, %f)\n", Start, End);
        exit(0);
    }
    float Delta = End - Start;
    float RandomValue = RandomFloat() * Delta + Start;
    return RandomValue;
}

inline uint32_t Log2OfPow2(uint32_t x)
{
	uint32_t ret = 0;

	while (x >>= 1)
		++ret;

	return ret;
}

struct Complex
{
	float a;
	float b;

	Complex() { a = b = 0; }
	Complex(float re, float im) { a = re; b = im; }

	Complex& operator +=(const Complex& other) { a += other.a; b += other.b; return *this; }

	inline Complex operator +(const Complex& other)	{ return Complex(a + other.a, b + other.b); }
	inline Complex operator -(const Complex& other)	{ return Complex(a - other.a, b - other.b); }
	inline Complex operator *(const Complex& other)	{ return Complex(a * other.a - b * other.b, b * other.a + a * other.b); }
};

#endif // !__QGE_MATH_H__