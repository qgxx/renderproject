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

struct Vector3
{
	float x, y, z;

	Vector3();
	Vector3(const Vector3& other);
	Vector3(float _x, float _y, float _z);
	Vector3(const float* values);

	Vector3 operator *(const Vector3& v) const;
	Vector3 operator +(const Vector3& v) const;
	Vector3 operator -(const Vector3& v) const;
	Vector3 operator *(float s) const;
	Vector3 operator /(float s) const;

	Vector3 operator -() const;

	Vector3& operator =(const Vector3& other);
	Vector3& operator +=(const Vector3& other);
	Vector3& operator *=(float s);

	static Vector3 Min(const Vector3& a, const Vector3& b);
	static Vector3 Max(const Vector3& a, const Vector3& b);

	inline operator float*()					{ return &x; }
	inline operator const float*() const		{ return &x; }

	inline float& operator [](int index)		{ return *(&x + index); }
	inline float operator [](int index) const	{ return *(&x + index); }
};

struct Vector4
{
	float x, y, z, w;

	Vector4();
	Vector4(const Vector4& other);
	Vector4(const Vector3& v, float w);
	Vector4(const Vector2& v, float z, float w);
	Vector4(float _x, float _y, float _z, float _w);
	Vector4(const float* values);

	Vector4 operator +(const Vector4& v) const;
	Vector4 operator -(const Vector4& v) const;
	Vector4 operator *(float s) const;
	Vector4 operator /(float s) const;

	Vector4& operator =(const Vector4& other);
	Vector4& operator /=(float s);

	inline operator float*()					{ return &x; }
	inline operator const float*() const		{ return &x; }

	inline float& operator [](int index)		{ return *(&x + index); }
	inline float operator [](int index) const	{ return *(&x + index); }
};

struct Matrix
{
	// NOTE: row-major (multiply with it from the right)

	float _11, _12, _13, _14;
	float _21, _22, _23, _24;
	float _31, _32, _33, _34;
	float _41, _42, _43, _44;	// translation goes here

	Matrix();
	Matrix(const Matrix& other);
	Matrix(float v11, float v22, float v33, float v44);
	Matrix(
		float v11, float v12, float v13, float v14,
		float v21, float v22, float v23, float v24,
		float v31, float v32, float v33, float v34,
		float v41, float v42, float v43, float v44);
	Matrix(const float* values);

	Matrix& operator =(const Matrix& other);

	inline operator float*()						{ return &_11; }
	inline operator const float*() const			{ return &_11; }

	inline float* operator [](int row)				{ return (&_11 + 4 * row); }
	inline const float* operator [](int row) const	{ return (&_11 + 4 * row); }
};

class AABox
{
public:
	Vector3 Min;
	Vector3 Max;

	AABox();
	AABox(const AABox& other);
	AABox(const Vector3& size);
	AABox(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax);

	AABox& operator =(const AABox& other);
	AABox operator +(const AABox& other);

	bool Contains(const Vector3& p) const;
	bool Intersects(const AABox& other) const;

	void Add(float x, float y, float z);
	void Add(const Vector3& p);
	void GetCenter(Vector3& out) const;
	void GetSize(Vector3& out) const;
	void GetHalfSize(Vector3& out) const;
	void GetPlanes(Vector4 out[6]) const;
	void Inset(float dx, float dy, float dz);
	void Offset(float dx, float dy, float dz);
	void TransformAxisAligned(const Matrix& m);
	void ToMatrix(Matrix& out) const;

	float Radius() const;
	float RayIntersect(const Vector3& start, const Vector3& dir) const;
	float Nearest(const Vector4& from) const;
	float Farthest(const Vector4& from) const;
};

void FrustumPlanes(Vector4 out[6], const Matrix& viewproj);
int FrustumIntersect(Vector4 frustum[6], const AABox& box);

void Vec3Subtract(Vector3& out, const Vector3& a, const Vector3& b);
float Vec3Length(const Vector3& v);


#endif // !__QGE_MATH_H__