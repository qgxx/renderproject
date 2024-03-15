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

struct Complex {
	float a;
	float b;

	Complex();
	Complex(float re, float im);

	Complex& operator +=(const Complex& other);

	inline Complex operator +(const Complex& other)	{ return Complex(a + other.a, b + other.b); }
	inline Complex operator -(const Complex& other)	{ return Complex(a - other.a, b - other.b); }
	inline Complex operator *(const Complex& other)	{ return Complex(a * other.a - b * other.b, b * other.a + a * other.b); }
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

void Vec2Normalize(Vector2& out, const Vector2& v);
float Vec2Length(const Vector2& v);
float Vec2Dot(const Vector2& a, const Vector2& b);

#endif // !__QGE_MATH_H__