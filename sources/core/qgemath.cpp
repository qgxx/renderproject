#include "qgemath.h"

Complex::Complex()
{
	a = b = 0;
}

Complex::Complex(float re, float im)
{
	a = re;
	b = im;
}

Complex& Complex::operator +=(const Complex& other)
{
	a += other.a;
	b += other.b;

	return *this;
}

Vector2::Vector2()
{
	x = y = 0;
}

Vector2::Vector2(const Vector2& other)
{
	x = other.x;
	y = other.y;
}

Vector2::Vector2(float _x, float _y)
{
	x = _x;
	y = _y;
}

Vector2::Vector2(const float* values)
{
	x = values[0];
	y = values[1];
}

Vector2 Vector2::operator +(const Vector2& v) const
{
	return Vector2(x + v.x, y + v.y);
}

Vector2 Vector2::operator -(const Vector2& v) const
{
	return Vector2(x - v.x, y - v.y);
}

Vector2 Vector2::operator *(float s) const
{
	return Vector2(x * s, y * s);
}

Vector2& Vector2::operator =(const Vector2& other)
{
	x = other.x;
	y = other.y;

	return *this;
}

void Vec2Normalize(Vector2& out, const Vector2& v)
{
	float il = 1.0f / sqrtf(v.x * v.x + v.y * v.y);

	out[0] = v[0] * il;
	out[1] = v[1] * il;
}

float Vec2Length(const Vector2& v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}

float Vec2Dot(const Vector2& a, const Vector2& b)
{
	return (a.x * b.x + a.y * b.y);
}