
#ifndef __EFFEKSEER_VEC2F_H__
#define __EFFEKSEER_VEC2F_H__

#include "Effekseer.SIMD4f.h"
#include "../Effekseer.Math.h"

namespace Effekseer
{

struct Vector2D;
struct vector2d;

struct Vec2f
{
	SIMD4f s;

	explicit Vec2f() = default;
	Vec2f(const Vec2f& vec) = default;
	Vec2f(float x, float y): s(x, y, 0.0f, 1.0f) {}
	Vec2f(const SIMD4f& vec): s(vec) {}
	Vec2f(const Vector2D& vec);
	Vec2f(const vector2d& vec);

	float GetX() const { return s.GetX(); }
	float GetY() const { return s.GetY(); }

	void SetX(float o) { s.SetX(o); }
	void SetY(float o) { s.SetY(o); }

	Vec2f& operator+=(const Vec2f& o) { s += o.s; return *this; }
	Vec2f& operator-=(const Vec2f& o) { s -= o.s; return *this; }
	Vec2f& operator*=(const Vec2f& o) { s *= o.s; return *this; }
	Vec2f& operator*=(float o) { s *= o; return *this; }
	Vec2f& operator/=(const Vec2f& o) { s /= o.s; return *this; }
	Vec2f& operator/=(float o) { s /= o; return *this; }

	float LengthSq() const;
	float Length() const;
	bool IsZero(float range = DefaultEpsilon) const;
	Vec2f Normalize() const;

	static Vec2f Load(const void* mem);
	static void Store(void* mem, const Vec2f& i);

	static Vec2f Sqrt(const Vec2f& i);
	static Vec2f Rsqrt(const Vec2f& i);
	static Vec2f Abs(const Vec2f& i);
	static Vec2f Min(const Vec2f& lhs, const Vec2f& rhs);
	static Vec2f Max(const Vec2f& lhs, const Vec2f& rhs);
	static bool Equal(const Vec2f& lhs, const Vec2f& rhs, float epsilon);
};

inline Vec2f operator+(const Vec2f& lhs, const Vec2f& rhs)
{
	return Vec2f{lhs.s + rhs.s};
}

inline Vec2f operator-(const Vec2f& lhs, const Vec2f& rhs)
{
	return Vec2f{lhs.s - rhs.s};
}

inline Vec2f operator*(const Vec2f& lhs, const Vec2f& rhs)
{
	return Vec2f{lhs.s * rhs.s};
}

inline Vec2f operator*(const Vec2f& lhs, float rhs)
{
	return Vec2f{lhs.s * rhs};
}

inline Vec2f operator/(const Vec2f& lhs, const Vec2f& rhs)
{
	return Vec2f{lhs.s / rhs.s};
}

inline Vec2f operator/(const Vec2f& lhs, float rhs)
{
	return Vec2f{lhs.s / rhs};
}

inline bool operator==(const Vec2f& lhs, const Vec2f& rhs)
{
	return (SIMD4f::MoveMask(SIMD4f::Equal(lhs.s, rhs.s)) & 0x03) == 0x3;
}

inline bool operator!=(const Vec2f& lhs, const Vec2f& rhs)
{
	return (SIMD4f::MoveMask(SIMD4f::Equal(lhs.s, rhs.s)) & 0x03) != 0x3;
}

inline Vec2f Vec2f::Load(const void* mem)
{
	return SIMD4f::Load2(mem);
}

inline void Vec2f::Store(void* mem, const Vec2f& i)
{
	SIMD4f::Store2(mem, i.s);
}

inline Vec2f Vec2f::Sqrt(const Vec2f& i)
{
	return Vec2f{SIMD4f::Sqrt(i.s)};
}

inline Vec2f Vec2f::Rsqrt(const Vec2f& i)
{
	return Vec2f{SIMD4f::Rsqrt(i.s)};
}

inline Vec2f Vec2f::Abs(const Vec2f& i)
{
	return Vec2f{SIMD4f::Abs(i.s)};
}

inline Vec2f Vec2f::Min(const Vec2f& lhs, const Vec2f& rhs)
{
	return Vec2f{SIMD4f::Min(lhs.s, rhs.s)};
}

inline Vec2f Vec2f::Max(const Vec2f& lhs, const Vec2f& rhs)
{
	return Vec2f{SIMD4f::Max(lhs.s, rhs.s)};
}

inline bool Vec2f::Equal(const Vec2f& lhs, const Vec2f& rhs, float epsilon)
{
	return (SIMD4f::MoveMask(SIMD4f::NearEqual(lhs.s, rhs.s, epsilon)) & 0x3) == 0x3;
}

inline float Vec2f::LengthSq() const
{
	auto o = s * s;
	return o.GetX() + o.GetY();
}

inline float Vec2f::Length() const
{
	return Effekseer::Sqrt(LengthSq());
}

inline bool Vec2f::IsZero(float range) const
{
	return LengthSq() < range * range;
}

inline Vec2f Vec2f::Normalize() const
{
	return *this * Effekseer::Rsqrt(LengthSq());
}

} // namespace Effekseer

#endif // __EFFEKSEER_VEC2F_H__