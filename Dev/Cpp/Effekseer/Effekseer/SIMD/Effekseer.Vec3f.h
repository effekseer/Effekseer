
#ifndef __EFFEKSEER_VEC3F_H__
#define __EFFEKSEER_VEC3F_H__

#include "Effekseer.SIMD4f.h"
#include "../Effekseer.Math.h"

namespace Effekseer
{

struct Vector3D;
struct vector3d;
struct Mat43f;
struct Mat44f;

struct Vec3f
{
	SIMD4f s;

	explicit Vec3f() = default;
	Vec3f(const Vec3f& vec) = default;
	Vec3f(float x, float y, float z): s(x, y, z, 1.0f) {}
	Vec3f(const SIMD4f& vec): s(vec) {}
	Vec3f(const Vector3D& vec);
	Vec3f(const vector3d& vec);

	float GetX() const { return s.GetX(); }
	float GetY() const { return s.GetY(); }
	float GetZ() const { return s.GetZ(); }

	void SetX(float o) { s.SetX(o); }
	void SetY(float o) { s.SetY(o); }
	void SetZ(float o) { s.SetZ(o); }

	Vec3f& operator+=(const Vec3f& o) { s += o.s; return *this; }
	Vec3f& operator-=(const Vec3f& o) { s -= o.s; return *this; }
	Vec3f& operator*=(const Vec3f& o) { s *= o.s; return *this; }
	Vec3f& operator*=(float o) { s *= o; return *this; }
	Vec3f& operator/=(const Vec3f& o) { s /= o.s; return *this; }
	Vec3f& operator/=(float o) { s /= o; return *this; }

	float GetSquaredLength() const;
	float GetLength() const;
	bool IsZero(float epsiron = DefaultEpsilon) const;
	Vec3f Normalize() const;
	Vec3f NormalizePrecisely() const;
	Vec3f NormalizeFast() const;

	static Vec3f Load(const void* mem);
	static void Store(void* mem, const Vec3f& i);

	static Vec3f Sqrt(const Vec3f& i);
	static Vec3f Rsqrt(const Vec3f& i);
	static Vec3f Abs(const Vec3f& i);
	static Vec3f Min(const Vec3f& lhs, const Vec3f& rhs);
	static Vec3f Max(const Vec3f& lhs, const Vec3f& rhs);
	static float Dot(const Vec3f& lhs, const Vec3f& rhs);
	static Vec3f Cross(const Vec3f& lhs, const Vec3f& rhs);
	static bool Equal(const Vec3f& lhs, const Vec3f& rhs, float epsilon = DefaultEpsilon);
	static Vec3f Transform(const Vec3f& lhs, const Mat43f& rhs);
	static Vec3f Transform(const Vec3f& lhs, const Mat44f& rhs);
};

inline Vec3f operator-(const Vec3f& i) { return Vec3f(-i.GetX(), -i.GetY(), -i.GetZ()); }

inline Vec3f operator+(const Vec3f& lhs, const Vec3f& rhs)
{
	return Vec3f{lhs.s + rhs.s};
}

inline Vec3f operator-(const Vec3f& lhs, const Vec3f& rhs)
{
	return Vec3f{lhs.s - rhs.s};
}

inline Vec3f operator*(const Vec3f& lhs, const Vec3f& rhs)
{
	return Vec3f{lhs.s * rhs.s};
}

inline Vec3f operator*(const Vec3f& lhs, float rhs)
{
	return Vec3f{lhs.s * rhs};
}

inline Vec3f operator/(const Vec3f& lhs, const Vec3f& rhs)
{
	return Vec3f{lhs.s / rhs.s};
}

inline Vec3f operator/(const Vec3f& lhs, float rhs)
{
	return Vec3f{lhs.s / rhs};
}

inline bool operator==(const Vec3f& lhs, const Vec3f& rhs)
{
	return (SIMD4f::MoveMask(SIMD4f::Equal(lhs.s, rhs.s)) & 0x07) == 0x7;
}

inline bool operator!=(const Vec3f& lhs, const Vec3f& rhs)
{
	return (SIMD4f::MoveMask(SIMD4f::Equal(lhs.s, rhs.s)) & 0x07) != 0x7;
}

inline Vec3f Vec3f::Load(const void* mem)
{
	return SIMD4f::Load3(mem);
}

inline void Vec3f::Store(void* mem, const Vec3f& i)
{
	SIMD4f::Store3(mem, i.s);
}

inline Vec3f Vec3f::Sqrt(const Vec3f& i)
{
	return Vec3f{SIMD4f::Sqrt(i.s)};
}

inline Vec3f Vec3f::Rsqrt(const Vec3f& i)
{
	return Vec3f{SIMD4f::Rsqrt(i.s)};
}

inline Vec3f Vec3f::Abs(const Vec3f& i)
{
	return Vec3f{SIMD4f::Abs(i.s)};
}

inline Vec3f Vec3f::Min(const Vec3f& lhs, const Vec3f& rhs)
{
	return Vec3f{SIMD4f::Min(lhs.s, rhs.s)};
}

inline Vec3f Vec3f::Max(const Vec3f& lhs, const Vec3f& rhs)
{
	return Vec3f{SIMD4f::Max(lhs.s, rhs.s)};
}

inline float Vec3f::Dot(const Vec3f& lhs, const Vec3f& rhs)
{
	return SIMD4f::Dot3(lhs.s, rhs.s).GetX();
}

inline Vec3f Vec3f::Cross(const Vec3f& lhs, const Vec3f& rhs)
{
	return SIMD4f::Cross3(lhs.s, rhs.s);
}

inline bool Vec3f::Equal(const Vec3f& lhs, const Vec3f& rhs, float epsilon)
{
	return (SIMD4f::MoveMask(SIMD4f::NearEqual(lhs.s, rhs.s, epsilon)) & 0x7) == 0x7;
}

inline float Vec3f::GetSquaredLength() const
{
	auto o = s * s;
	return o.GetX() + o.GetY() + o.GetZ();
}

inline float Vec3f::GetLength() const
{
	return Effekseer::Sqrt(GetSquaredLength());
}

inline bool Vec3f::IsZero(float epsiron) const
{
	return (SIMD4f::MoveMask(SIMD4f::IsZero(s, epsiron)) & 0x7) == 0x7;
}

inline Vec3f Vec3f::Normalize() const
{
	return *this * Effekseer::Rsqrt(GetSquaredLength());
}

inline Vec3f Vec3f::NormalizePrecisely() const { return *this / Effekseer::Sqrt(GetSquaredLength()); }

inline Vec3f Vec3f::NormalizeFast() const { return *this * Effekseer::Rsqrt(GetSquaredLength()); }

} // namespace Effekseer

#endif // __EFFEKSEER_VEC3F_H__