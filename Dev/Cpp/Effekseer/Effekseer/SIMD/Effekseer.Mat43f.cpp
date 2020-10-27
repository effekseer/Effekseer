#include "Effekseer.Mat43f.h"
#include "../Effekseer.Matrix43.h"
#include <cmath>

namespace Effekseer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const Mat43f Mat43f::Identity = Mat43f(
	1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0);

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Mat43f::Mat43f(const Matrix43& mat)
{
	X = SIMD4f::Load3(mat.Value[0]);
	Y = SIMD4f::Load3(mat.Value[1]);
	Z = SIMD4f::Load3(mat.Value[2]);
	SIMD4f W = SIMD4f::Load3(mat.Value[3]);
	SIMD4f::Transpose(X, Y, Z, W);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Mat43f::IsValid() const
{
	const SIMD4f nan{NAN};
	const SIMD4f inf{INFINITY};
	SIMD4f res =
		SIMD4f::Equal(X, nan) |
		SIMD4f::Equal(Y, nan) |
		SIMD4f::Equal(Z, nan) |
		SIMD4f::Equal(X, inf) |
		SIMD4f::Equal(Y, inf) |
		SIMD4f::Equal(Z, inf);
	return SIMD4f::MoveMask(res) == 0;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Vec3f Mat43f::GetScale() const
{
	SIMD4f x2 = X * X;
	SIMD4f y2 = Y * Y;
	SIMD4f z2 = Z * Z;
	SIMD4f s2 = x2 + y2 + z2;
	SIMD4f sq = SIMD4f::Sqrt(s2);
	return Vec3f{sq};
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Mat43f Mat43f::GetRotation() const
{
	SIMD4f x2 = X * X;
	SIMD4f y2 = Y * Y;
	SIMD4f z2 = Z * Z;
	SIMD4f s2 = x2 + y2 + z2;
	SIMD4f rsq = SIMD4f::Rsqrt(s2);
	rsq.SetW(0.0f);

	Mat43f ret;
	ret.X = X * rsq;
	ret.Y = Y * rsq;
	ret.Z = Z * rsq;
	return ret;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Vec3f Mat43f::GetTranslation() const
{
	return Vec3f(X.GetW(), Y.GetW(), Z.GetW());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Mat43f::GetSRT(Vec3f& s, Mat43f& r, Vec3f& t) const
{
	SIMD4f x2 = X * X;
	SIMD4f y2 = Y * Y;
	SIMD4f z2 = Z * Z;
	SIMD4f s2 = x2 + y2 + z2;

	if (Vec3f(s2).IsZero())
	{
		s = Vec3f(0.0f);
		r = Mat43f::Identity;
	}
	else
	{
		SIMD4f rsq = SIMD4f::Rsqrt(s2);
		rsq.SetW(0.0f);

		s = SIMD4f(1.0f) / rsq;
		r.X = X * rsq;
		r.Y = Y * rsq;
		r.Z = Z * rsq;
	}

	t = Vec3f(X.GetW(), Y.GetW(), Z.GetW());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Mat43f::SetTranslation(const Vec3f& t)
{
	X.SetW(t.GetX());
	Y.SetW(t.GetY());
	Z.SetW(t.GetZ());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Mat43f::Equal(const Mat43f& lhs, const Mat43f& rhs, float epsilon)
{
	SIMD4f ret =
		SIMD4f::NearEqual(lhs.X, rhs.X, epsilon) &
		SIMD4f::NearEqual(lhs.Y, rhs.Y, epsilon) &
		SIMD4f::NearEqual(lhs.Z, rhs.Z, epsilon);
	return (SIMD4f::MoveMask(ret) & 0xf) == 0xf;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Mat43f Mat43f::SRT(const Vec3f& s, const Mat43f& r, const Vec3f& t)
{
	Mat43f ret;
	ret.X = r.X * s.s;
	ret.Y = r.Y * s.s;
	ret.Z = r.Z * s.s;
	ret.X.SetW(t.GetX());
	ret.Y.SetW(t.GetY());
	ret.Z.SetW(t.GetZ());
	return ret;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Mat43f Mat43f::Scaling(float x, float y, float z)
{
	Mat43f ret;
	ret.X = {x, 0.0f, 0.0f, 0.0f};
	ret.Y = {0.0f, y, 0.0f, 0.0f};
	ret.Z = {0.0f, 0.0f, z, 0.0f};
	return ret;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Mat43f Mat43f::Scaling(const Vec3f& scale)
{
	Mat43f ret;
	ret.X = {scale.GetX(), 0.0f, 0.0f, 0.0f};
	ret.Y = {0.0f, scale.GetY(), 0.0f, 0.0f};
	ret.Z = {0.0f, 0.0f, scale.GetZ(), 0.0f};
	return ret;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Mat43f Mat43f::RotationX(float angle)
{
	float c, s;
	::Effekseer::SinCos(angle, s, c);

	Mat43f ret;
	ret.X = {1.0f, 0.0f, 0.0f, 0.0f};
	ret.Y = {0.0f, c, -s, 0.0f};
	ret.Z = {0.0f, s, c, 0.0f};
	return ret;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Mat43f Mat43f::RotationY(float angle)
{
	float c, s;
	::Effekseer::SinCos(angle, s, c);

	Mat43f ret;
	ret.X = {c, 0.0f, s, 0.0f};
	ret.Y = {0.0f, 1.0f, 0.0f, 0.0f};
	ret.Z = {-s, 0.0f, c, 0.0f};
	return ret;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Mat43f Mat43f::RotationZ(float angle)
{
	float c, s;
	::Effekseer::SinCos(angle, s, c);

	Mat43f ret;
	ret.X = {c, -s, 0.0f, 0.0f};
	ret.Y = {s, c, 0.0f, 0.0f};
	ret.Z = {0.0f, 0.0f, 1.0f, 0.0f};
	return ret;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Mat43f Mat43f::RotationXYZ(float rx, float ry, float rz)
{
	float cx, sx, cy, sy, cz, sz;

	if (rx != 0.0f)
	{
		::Effekseer::SinCos(rx, sx, cx);
	}
	else
	{
		sx = 0.0f;
		cx = 1.0f;
	}
	if (ry != 0.0f)
	{
		::Effekseer::SinCos(ry, sy, cy);
	}
	else
	{
		sy = 0.0f;
		cy = 1.0f;
	}
	if (rz != 0.0f)
	{
		::Effekseer::SinCos(rz, sz, cz);
	}
	else
	{
		sz = 0.0f;
		cz = 1.0f;
	}

	float m00 = cy * cz;
	float m01 = cy * sz;
	float m02 = -sy;

	float m10 = sx * sy * -sz + cx * -sz;
	float m11 = sx * sy * sz + cx * cz;
	float m12 = sx * cy;

	float m20 = cx * sy * cz + sx * sz;
	float m21 = cx * sy * sz - sx * cz;
	float m22 = cx * cy;

	Mat43f ret;
	ret.X = {m00, m10, m20, 0.0f};
	ret.Y = {m01, m11, m21, 0.0f};
	ret.Z = {m02, m12, m22, 0.0f};
	return ret;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Mat43f Mat43f::RotationZXY(float rz, float rx, float ry)
{
	float cx, sx, cy, sy, cz, sz;

	if (rx != 0.0f)
	{
		::Effekseer::SinCos(rx, sx, cx);
	}
	else
	{
		sx = 0.0f;
		cx = 1.0f;
	}
	if (ry != 0.0f)
	{
		::Effekseer::SinCos(ry, sy, cy);
	}
	else
	{
		sy = 0.0f;
		cy = 1.0f;
	}
	if (rz != 0.0f)
	{
		::Effekseer::SinCos(rz, sz, cz);
	}
	else
	{
		sz = 0.0f;
		cz = 1.0f;
	}

	float m00 = cz * cy + sz * sx * sy;
	float m01 = sz * cx;
	float m02 = cz * -sy + sz * sx * cy;

	float m10 = -sz * cy + cz * sx * sy;
	float m11 = cz * cx;
	float m12 = -sz * -sy + cz * sx * cy;

	float m20 = cx * sy;
	float m21 = -sx;
	float m22 = cx * cy;

	Mat43f ret;
	ret.X = {m00, m10, m20, 0.0f};
	ret.Y = {m01, m11, m21, 0.0f};
	ret.Z = {m02, m12, m22, 0.0f};
	return ret;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Mat43f Mat43f::RotationAxis(const Vec3f& axis, float angle)
{
	const float c = cosf(angle);
	const float s = sinf(angle);
	return RotationAxis(axis, s, c);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Mat43f Mat43f::RotationAxis(const Vec3f& axis, float s, float c)
{
	const float cc = 1.0f - c;

	float m00 = cc * (axis.GetX() * axis.GetX()) + c;
	float m01 = cc * (axis.GetX() * axis.GetY()) + (axis.GetZ() * s);
	float m02 = cc * (axis.GetZ() * axis.GetX()) - (axis.GetY() * s);

	float m10 = cc * (axis.GetX() * axis.GetY()) - (axis.GetZ() * s);
	float m11 = cc * (axis.GetY() * axis.GetY()) + c;
	float m12 = cc * (axis.GetY() * axis.GetZ()) + (axis.GetX() * s);

	float m20 = cc * (axis.GetZ() * axis.GetX()) + (axis.GetY() * s);
	float m21 = cc * (axis.GetY() * axis.GetZ()) - (axis.GetX() * s);
	float m22 = cc * (axis.GetZ() * axis.GetZ()) + c;

	Mat43f ret;
	ret.X = {m00, m10, m20, 0.0f};
	ret.Y = {m01, m11, m21, 0.0f};
	ret.Z = {m02, m12, m22, 0.0f};
	return ret;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Mat43f Mat43f::Translation(float x, float y, float z)
{
	Mat43f ret;
	ret.X = {1.0f, 0.0f, 0.0f, x};
	ret.Y = {0.0f, 1.0f, 0.0f, y};
	ret.Z = {0.0f, 0.0f, 1.0f, z};
	return ret;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Mat43f Mat43f::Translation(const Vec3f& pos)
{
	Mat43f ret;
	ret.X = {1.0f, 0.0f, 0.0f, pos.GetX()};
	ret.Y = {0.0f, 1.0f, 0.0f, pos.GetY()};
	ret.Z = {0.0f, 0.0f, 1.0f, pos.GetZ()};
	return ret;
}

} // namespace Effekseer
