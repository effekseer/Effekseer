
#ifndef __EFFEKSEER_VEC4F_H__
#define __EFFEKSEER_VEC4F_H__

#include "Effekseer.SIMD4f.h"

namespace Effekseer
{

struct Vec4f
{
	SIMD4f s;

	float GetX() const { return s.GetX(); }
	float GetY() const { return s.GetY(); }
	float GetZ() const { return s.GetZ(); }
	float GetW() const { return s.GetW(); }

	void SetX(float o) { s.SetX(o); }
	void SetY(float o) { s.SetY(o); }
	void SetZ(float o) { s.SetZ(o); }
	void SetW(float o) { s.SetW(o); }

	Vec4f& operator+=(const Vec4f& o)
	{
		this->s = this->s + o.s;
		return *this;
	}

	Vec4f& operator-=(const Vec4f& o)
	{
		this->s = this->s - o.s;
		return *this;
	}

	Vec4f& operator*=(const Vec4f& o)
	{
		this->s = this->s * o.s;
		return *this;
	}

	Vec4f& operator/=(const Vec4f& o)
	{
		this->s = this->s / o.s;
		return *this;
	}
};

inline Vec4f operator+(const Vec4f& lhs, const Vec4f& rhs) { return Vec4f{lhs.s + rhs.s}; }

inline Vec4f operator-(const Vec4f& lhs, const Vec4f& rhs) { return Vec4f{lhs.s - rhs.s}; }

inline Vec4f operator*(const Vec4f& lhs, const Vec4f& rhs) { return Vec4f{lhs.s * rhs.s}; }

inline Vec4f operator/(const Vec4f& lhs, const Vec4f& rhs) { return Vec4f{lhs.s / rhs.s}; }

inline Vec4f sqrt(const Vec4f& o) { return Vec4f{sqrt(o.s)}; }

} // namespace Effekseer

#endif // __EFFEKSEER_VEC4F_H__