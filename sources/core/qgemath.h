#ifndef __QGE_MATH_H__
#define __QGE_MATH_H__

#include <iostream>
#include <stdlib.h>
#include <glm/glm.hpp>

#define powi(base,exp) (int)powf((float)(base), (float)(exp))

static const float TWO_PI = 6.283185f;
static const float ONE_OVER_SQRT_2 = 0.7071067f;

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

namespace Math {

static const float PI = 3.141592f;
static const float TWO_PI = 6.283185f;
static const float HALF_PI = 1.570796f;
static const float ONE_OVER_SQRT_2 = 0.7071067f;
static const float ONE_OVER_SQRT_TWO_PI = 0.39894228f;

class Color
{
public:
	float r, g, b, a;

	Color();
	Color(float _r, float _g, float _b, float _a);
	Color(uint32_t argb32);

	Color operator *(float f);

	Color& operator =(const Color& other);
	Color& operator *=(const Color& other);

	static Color Lerp(const Color& from, const Color& to, float frac);
	static Color sRGBToLinear(uint32_t argb32);
	static Color sRGBToLinear(uint8_t red, uint8_t green, uint8_t blue);
	static Color sRGBToLinear(float red, float green, float blue, float alpha);

	static inline uint8_t ArgbA32(uint32_t c)	{ return ((uint8_t)((c >> 24) & 0xff)); }
	static inline uint8_t ArgbR32(uint32_t c)	{ return ((uint8_t)((c >> 16) & 0xff)); }
	static inline uint8_t ArgbG32(uint32_t c)	{ return ((uint8_t)((c >> 8) & 0xff)); }
	static inline uint8_t ArgbB32(uint32_t c)	{ return ((uint8_t)(c & 0xff)); }

	operator uint32_t() const;

	inline operator float*()					{ return &r; }
	inline operator const float*() const		{ return &r; }
};

struct Vector2
{
	float x, y;

	Vector2();
	Vector2(const Vector2& other);
	Vector2(float _x, float _y);
	Vector2(const float* values);

	Vector2 operator +(const Vector2& v) const;
	Vector2 operator -(const Vector2& v) const;
	Vector2 operator *(float s) const;

	Vector2& operator =(const Vector2& other);

	inline operator float*()					{ return &x; }
	inline operator const float*() const		{ return &x; }

	inline float& operator [](int index)		{ return *(&x + index); }
	inline float operator [](int index) const	{ return *(&x + index); }
};

struct Complex
{
	float a;
	float b;

	Complex();
	Complex(float re, float im);

	Complex& operator +=(const Complex& other);

	inline Complex operator +(const Complex& other)	{ return Complex(a + other.a, b + other.b); }
	inline Complex operator -(const Complex& other)	{ return Complex(a - other.a, b - other.b); }
	inline Complex operator *(const Complex& other)	{ return Complex(a * other.a - b * other.b, b * other.a + a * other.b); }
};

inline float Vec2Length(const Vector2& v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}

inline float Vec2Distance(const Vector2& a, const Vector2& b)
{
	return Vec2Length(a - b);
}

inline void Vec2Normalize(Vector2& out, const Vector2& v)
{
	float il = 1.0f / sqrtf(v.x * v.x + v.y * v.y);

	out[0] = v[0] * il;
	out[1] = v[1] * il;
}

inline void Vec2Subtract(Vector2& out, const Vector2& a, const Vector2& b)
{
	out.x = a.x - b.x;
	out.y = a.y - b.y;
}

inline float Vec2Dot(const Vector2& a, const Vector2& b)
{
	return (a.x * b.x + a.y * b.y);
}

inline uint32_t Log2OfPow2(uint32_t x)
{
	uint32_t ret = 0;

	while (x >>= 1)
		++ret;

	return ret;
}
}

#endif // !__QGE_MATH_H__