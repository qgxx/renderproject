#include "qgemath.h"

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

Vector3::Vector3()
{
	x = y = z = 0;
}

Vector3::Vector3(const Vector3& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
}

Vector3::Vector3(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

Vector3::Vector3(const float* values)
{
	x = values[0];
	y = values[1];
	z = values[2];
}

Vector3 Vector3::operator *(const Vector3& v) const
{
	return Vector3(x * v.x, y * v.y, z * v.z);
}

Vector3 Vector3::operator +(const Vector3& v) const
{
	return Vector3(x + v.x, y + v.y, z + v.z);
}

Vector3 Vector3::operator -(const Vector3& v) const
{
	return Vector3(x - v.x, y - v.y, z - v.z);
}

Vector3 Vector3::operator *(float s) const
{
	return Vector3(x * s, y * s, z * s);
}

Vector3 Vector3::operator /(float s) const
{
	return Vector3(x / s, y / s, z / s);
}

Vector3 Vector3::operator -() const
{
	return Vector3(-x, -y, -z);
}

Vector3& Vector3::operator =(const Vector3& other)
{
	x = other.x;
	y = other.y;
	z = other.z;

	return *this;
}

Vector3& Vector3::operator +=(const Vector3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}

Vector3& Vector3::operator *=(float s)
{
	x *= s;
	y *= s;
	z *= s;

	return *this;
}

Vector3 Vector3::Min(const Vector3& a, const Vector3& b)
{
	return Vector3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

Vector3 Vector3::Max(const Vector3& a, const Vector3& b)
{
	return Vector3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

Vector4::Vector4()
{
	x = y = z = w = 0;
}

Vector4::Vector4(const Vector4& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	w = other.w;
}

Vector4::Vector4(const Vector3& v, float w)
{
	x = v.x;
	y = v.y;
	z = v.z;

	this->w = w;
}

Vector4::Vector4(const Vector2& v, float z, float w)
{
	x = v.x;
	y = v.y;

	this->z = z;
	this->w = w;
}

Vector4::Vector4(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

Vector4::Vector4(const float* values)
{
	x = values[0];
	y = values[1];
	z = values[2];
	w = values[3];
}

Vector4 Vector4::operator +(const Vector4& v) const
{
	return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
}

Vector4 Vector4::operator -(const Vector4& v) const
{
	return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
}

Vector4 Vector4::operator *(float s) const
{
	return Vector4(x * s, y * s, z * s, w * s);
}

Vector4 Vector4::operator /(float s) const
{
	return Vector4(x / s, y / s, z / s, w / s);
}

Vector4& Vector4::operator =(const Vector4& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	w = other.w;

	return *this;
}

Vector4& Vector4::operator /=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;

	return *this;
}

Matrix::Matrix()
{
	_11 = _12 = _13 = _14 = 0;
	_21 = _22 = _23 = _24 = 0;
	_31 = _32 = _33 = _34 = 0;
	_41 = _42 = _43 = _44 = 0;
}

Matrix::Matrix(const Matrix& other)
{
	operator =(other);
}

Matrix::Matrix(float v11, float v22, float v33, float v44)
{
	_12 = _13 = _14 = 0;
	_21 = _23 = _24 = 0;
	_31 = _32 = _34 = 0;
	_41 = _42 = _43 = 0;

	_11 = v11;
	_22 = v22;
	_33 = v33;
	_44 = v44;
}

Matrix::Matrix(
	float v11, float v12, float v13, float v14,
	float v21, float v22, float v23, float v24,
	float v31, float v32, float v33, float v34,
	float v41, float v42, float v43, float v44)
{
	_11 = v11;	_21 = v21;	_31 = v31;	_41 = v41;
	_12 = v12;	_22 = v22;	_32 = v32;	_42 = v42;
	_13 = v13;	_23 = v23;	_33 = v33;	_43 = v43;
	_14 = v14;	_24 = v24;	_34 = v34;	_44 = v44;
}

Matrix::Matrix(const float* values)
{
	_11 = values[0];	_21 = values[4];
	_12 = values[1];	_22 = values[5];
	_13 = values[2];	_23 = values[6];
	_14 = values[3];	_24 = values[7];

	_31 = values[8];	_41 = values[12];
	_32 = values[9];	_42 = values[13];
	_33 = values[10];	_43 = values[14];
	_34 = values[11];	_44 = values[15];
}

Matrix& Matrix::operator =(const Matrix& other)
{
	_11 = other._11;	_21 = other._21;
	_12 = other._12;	_22 = other._22;
	_13 = other._13;	_23 = other._23;
	_14 = other._14;	_24 = other._24;

	_31 = other._31;	_41 = other._41;
	_32 = other._32;	_42 = other._42;
	_33 = other._33;	_43 = other._43;
	_34 = other._34;	_44 = other._44;

	return *this;
}

static void Assign_If_Less(float& a, float b)
{
	if (a < b)
		a = b;
};

static void Assign_If_Greater(float& a, float b)
{
	if (a > b)
		a = b;
};

static void CalculateDistanceIncr(float& value, const Vector3& p, const Vector4& from)
{
	float d = from.x * p.x + from.y * p.y + from.z * p.z + from.w;
	Assign_If_Greater(value, d);
}

static void CalculateDistanceDecr(float& value, const Vector3& p, const Vector4& from)
{
	float d = from.x * p.x + from.y * p.y + from.z * p.z + from.w;
	Assign_If_Less(value, d);
}

static void Vec3TransformCoord(Vector3& out, const Vector3& v, const Matrix& m)
{
	Vector4 tmp;

	tmp[0] = v[0] * m._11 + v[1] * m._21 + v[2] * m._31 + m._41;
	tmp[1] = v[0] * m._12 + v[1] * m._22 + v[2] * m._32 + m._42;
	tmp[2] = v[0] * m._13 + v[1] * m._23 + v[2] * m._33 + m._43;
	tmp[3] = v[0] * m._14 + v[1] * m._24 + v[2] * m._34 + m._44;

	out[0] = tmp[0] / tmp[3];
	out[1] = tmp[1] / tmp[3];
	out[2] = tmp[2] / tmp[3];
}

static void MatrixIdentity(Matrix& out)
{
	out._12 = out._13 = out._14 = 0;
	out._21 = out._23 = out._24 = 0;
	out._31 = out._32 = out._34 = 0;
	out._41 = out._42 = out._43 = 0;

	out._11 = out._22 = out._33 = out._44 = 1;
}

static float Vec3Dot(const Vector3& a, const Vector3& b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

static void PlaneFromNormalAndPoint(Vector4& out, const Vector3& n, const Vector3& p)
{
	out.x = n.x;
	out.y = n.y;
	out.z = n.z;
	out.w = -Vec3Dot(p, n);
}

float Vec3Length(const Vector3& v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

static float Vec3Distance(const Vector3& a, const Vector3& b)
{
	return Vec3Length(a - b);
}

void Vec3Subtract(Vector3& out, const Vector3& a, const Vector3& b)
{
	out[0] = a[0] - b[0];
	out[1] = a[1] - b[1];
	out[2] = a[2] - b[2];
}

AABox::AABox()
{
	Min[0] = Min[1] = Min[2] = FLT_MAX;
	Max[0] = Max[1] = Max[2] = -FLT_MAX;
}

AABox::AABox(const AABox& other)
{
	operator =(other);
}

AABox::AABox(const Vector3& size)
{
	Min = size * -0.5f;
	Max = size * 0.5f;
}

AABox::AABox(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax)
{
	Min = Vector3(xmin, ymin, zmin);
	Max = Vector3(xmax, ymax, zmax);
}

AABox& AABox::operator =(const AABox& other)
{
	if (&other != this) {
		Min[0] = other.Min[0];
		Min[1] = other.Min[1];
		Min[2] = other.Min[2];

		Max[0] = other.Max[0];
		Max[1] = other.Max[1];
		Max[2] = other.Max[2];
	}

	return *this;
}

AABox AABox::operator +(const AABox& other)
{
	AABox ret = *this;

	ret.Add(other.Min);
	ret.Add(other.Max);

	return ret;
}

bool AABox::Contains(const Vector3& p) const
{
	return (
		(p.x >= Min.x && p.x <= Max.x) &&
		(p.y >= Min.y && p.y <= Max.y) &&
		(p.z >= Min.z && p.z <= Max.z));
}

bool AABox::Intersects(const AABox& other) const
{
	if (Max[0] < other.Min[0] || Min[0] > other.Max[0])
		return false;

	if (Max[1] < other.Min[1] || Min[1] > other.Max[1])
		return false;

	if (Max[2] < other.Min[2] || Min[2] > other.Max[2])
		return false;

	return true;
}

void AABox::Add(float x, float y, float z)
{
	Assign_If_Less(Max[0], x);
	Assign_If_Less(Max[1], y);
	Assign_If_Less(Max[2], z);

	Assign_If_Greater(Min[0], x);
	Assign_If_Greater(Min[1], y);
	Assign_If_Greater(Min[2], z);
}

void AABox::Add(const Vector3& p)
{
	Assign_If_Less(Max[0], p[0]);
	Assign_If_Less(Max[1], p[1]);
	Assign_If_Less(Max[2], p[2]);

	Assign_If_Greater(Min[0], p[0]);
	Assign_If_Greater(Min[1], p[1]);
	Assign_If_Greater(Min[2], p[2]);
}

void AABox::GetCenter(Vector3& out) const
{
	out[0] = (Min[0] + Max[0]) * 0.5f;
	out[1] = (Min[1] + Max[1]) * 0.5f;
	out[2] = (Min[2] + Max[2]) * 0.5f;
}

void AABox::GetSize(Vector3& out) const
{
	out[0] = Max[0] - Min[0];
	out[1] = Max[1] - Min[1];
	out[2] = Max[2] - Min[2];
}

void AABox::GetHalfSize(Vector3& out) const
{
	out[0] = (Max[0] - Min[0]) * 0.5f;
	out[1] = (Max[1] - Min[1]) * 0.5f;
	out[2] = (Max[2] - Min[2]) * 0.5f;
}

void AABox::Inset(float dx, float dy, float dz)
{
	Min[0] += dx;
	Min[1] += dy;
	Min[2] += dz;

	Max[0] -= dx;
	Max[1] -= dy;
	Max[2] -= dz;
}

void AABox::Offset(float dx, float dy, float dz)
{
	Min[0] += dx;
	Min[1] += dy;
	Min[2] += dz;

	Max[0] += dx;
	Max[1] += dy;
	Max[2] += dz;
}

void AABox::TransformAxisAligned(const Matrix& m)
{
	Vector3 vertices[8] = {
		{ Min[0], Min[1], Min[2] },
		{ Min[0], Min[1], Max[2] },
		{ Min[0], Max[1], Min[2] },
		{ Min[0], Max[1], Max[2] },
		{ Max[0], Min[1], Min[2] },
		{ Max[0], Min[1], Max[2] },
		{ Max[0], Max[1], Min[2] },
		{ Max[0], Max[1], Max[2] }
	};
	
	for (int i = 0; i < 8; ++i)
		Vec3TransformCoord(vertices[i], vertices[i], m);

	Min[0] = Min[1] = Min[2] = FLT_MAX;
	Max[0] = Max[1] = Max[2] = -FLT_MAX;

	for (int i = 0; i < 8; ++i)
		Add(vertices[i]);
}

void AABox::ToMatrix(Matrix& out) const
{
	MatrixIdentity(out);

	out._11 = (Max.x - Min.x) * 0.5f;
	out._22 = (Max.y - Min.y) * 0.5f;
	out._33 = (Max.z - Min.z) * 0.5f;

	out._41 = (Max.x + Min.x) * 0.5f;
	out._42 = (Max.y + Min.y) * 0.5f;
	out._43 = (Max.z + Min.z) * 0.5f;
}

void AABox::GetPlanes(Vector4 out[6]) const
{
	PlaneFromNormalAndPoint(out[0], { 1, 0, 0 }, { Min[0], Min[1], Min[2] });	// left
	PlaneFromNormalAndPoint(out[1], { -1, 0, 0 }, { Max[0], Min[1], Min[2] });	// right
	PlaneFromNormalAndPoint(out[2], { 0, 1, 0 }, { Min[0], Min[1], Min[2] });	// bottom
	PlaneFromNormalAndPoint(out[3], { 0, -1, 0 }, { Min[0], Max[1], Min[2] });	// top
	PlaneFromNormalAndPoint(out[4], { 0, 0, -1 }, { Min[0], Min[1], Max[2] });	// front
	PlaneFromNormalAndPoint(out[5], { 0, 0, 1 }, { Min[0], Min[1], Min[2] });	// back
}

float AABox::Radius() const
{
	return Vec3Distance(Min, Max) * 0.5f;
}

float AABox::RayIntersect(const Vector3& start, const Vector3& dir) const
{
	Vector3 m1, m2;
	float t1, t2, t3, t4, t5, t6;

	Vec3Subtract(m1, Min, start);
	Vec3Subtract(m2, Max, start);

	if (dir[0] == 0) {
		t1 = (m1[0] >= 0 ? FLT_MAX : -FLT_MAX);
		t2 = (m2[0] >= 0 ? FLT_MAX : -FLT_MAX);
	} else {
		t1 = m1[0] / dir[0];
		t2 = m2[0] / dir[0];
	}

	if (dir[1] == 0) {
		t3 = (m1[1] >= 0 ? FLT_MAX : -FLT_MAX);
		t4 = (m2[1] >= 0 ? FLT_MAX : -FLT_MAX);
	} else {
		t3 = m1[1] / dir[1];
		t4 = m2[1] / dir[1];
	}

	if (dir[2] == 0) {
		t5 = (m1[2] >= 0 ? FLT_MAX : -FLT_MAX);
		t6 = (m2[2] >= 0 ? FLT_MAX : -FLT_MAX);
	} else {
		t5 = m1[2] / dir[2];
		t6 = m2[2] / dir[2];
	}

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	if (tmax < 0 || tmin > tmax)
		return FLT_MAX;

	return tmin;
}

float AABox::Nearest(const Vector4& from) const
{
	float dist = FLT_MAX;

	CalculateDistanceIncr(dist, { Min[0], Min[1], Min[2] }, from);
	CalculateDistanceIncr(dist, { Min[0], Min[1], Max[2] }, from);
	CalculateDistanceIncr(dist, { Min[0], Max[1], Min[2] }, from);
	CalculateDistanceIncr(dist, { Min[0], Max[1], Max[2] }, from);
	CalculateDistanceIncr(dist, { Max[0], Min[1], Min[2] }, from);
	CalculateDistanceIncr(dist, { Max[0], Min[1], Max[2] }, from);
	CalculateDistanceIncr(dist, { Max[0], Max[1], Min[2] }, from);
	CalculateDistanceIncr(dist, { Max[0], Max[1], Max[2] }, from);

	return dist;
}

float AABox::Farthest(const Vector4& from) const
{
	float dist = -FLT_MAX;

	CalculateDistanceDecr(dist, { Min[0], Min[1], Min[2] }, from);
	CalculateDistanceDecr(dist, { Min[0], Min[1], Max[2] }, from);
	CalculateDistanceDecr(dist, { Min[0], Max[1], Min[2] }, from);
	CalculateDistanceDecr(dist, { Min[0], Max[1], Max[2] }, from);
	CalculateDistanceDecr(dist, { Max[0], Min[1], Min[2] }, from);
	CalculateDistanceDecr(dist, { Max[0], Min[1], Max[2] }, from);
	CalculateDistanceDecr(dist, { Max[0], Max[1], Min[2] }, from);
	CalculateDistanceDecr(dist, { Max[0], Max[1], Max[2] }, from);

	return dist;
}

static void PlaneNormalize(Vector4& out, const Vector4& p)
{
	float il = 1.0f / Vec3Length((const Vector3&)p);

	out[0] = p[0] * il;
	out[1] = p[1] * il;
	out[2] = p[2] * il;
	out[3] = p[3] * il;
}

void FrustumPlanes(Vector4 out[6], const Matrix& viewproj) {
	out[0] = { viewproj._11 + viewproj._14, viewproj._21 + viewproj._24, viewproj._31 + viewproj._34, viewproj._41 + viewproj._44 };	// left
	out[1] = { viewproj._14 - viewproj._11, viewproj._24 - viewproj._21, viewproj._34 - viewproj._31, viewproj._44 - viewproj._41 };	// right
	out[2] = { viewproj._14 - viewproj._12, viewproj._24 - viewproj._22, viewproj._34 - viewproj._32, viewproj._44 - viewproj._42 };	// top
	out[3] = { viewproj._12 + viewproj._14, viewproj._22 + viewproj._24, viewproj._32 + viewproj._34, viewproj._42 + viewproj._44 };	// bottom
	out[4] = { viewproj._14 + viewproj._13, viewproj._24 + viewproj._23, viewproj._34 + viewproj._33, viewproj._44 + viewproj._43 };	// near
	out[5] = { viewproj._14 - viewproj._13, viewproj._24 - viewproj._23, viewproj._34 - viewproj._33, viewproj._44 - viewproj._43 };	// far

	PlaneNormalize(out[0], out[0]);
	PlaneNormalize(out[1], out[1]);
	PlaneNormalize(out[2], out[2]);
	PlaneNormalize(out[3], out[3]);
	PlaneNormalize(out[4], out[4]);
	PlaneNormalize(out[5], out[5]);
}

static float PlaneDotCoord(const Vector4& plane, const Vector3& p)
{
	return (plane.x * p.x + plane.y * p.y + plane.z * p.z + plane.w);
}

int FrustumIntersect(Vector4 frustum[6], const AABox& box) {
	Vector3 center, halfsize;
	float dist, maxdist;
	int result = 2; // inside

	box.GetCenter(center);
	box.GetHalfSize(halfsize);
	
	// NOTE: fast version, might give false negatives
	for (int j = 0; j < 6; ++j) {
		const Vector4& plane = frustum[j];

		dist = PlaneDotCoord(plane, center);
		maxdist = fabs(plane[0] * halfsize[0]) + fabs(plane[1] * halfsize[1]) + fabs(plane[2] * halfsize[2]);

		if (dist < -maxdist)
			return 0;	// outside
		else if (fabs(dist) < maxdist)
			result = 1;	// intersect
	}

	return result;
}