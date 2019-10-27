
#ifndef __EFFEKSEER_VEC3F_H__
#define __EFFEKSEER_VEC3F_H__

#include "Effekseer.SIMD4f.h"

namespace Effekseer
{

struct Vec3f
{
	SIMD4f s;

	float GetX() const { return s.GetX(); }
	float GetY() const { return s.GetY(); }
	float GetZ() const { return s.GetZ(); }

	void SetX(float o) { s.SetX(o); }
	void SetY(float o) { s.SetY(o); }
	void SetZ(float o) { s.SetZ(o); }
};

inline Vec3f operator+(const Vec3f& lhs, const Vec3f& rhs) { return Vec3f{lhs.s + rhs.s}; }

inline Vec3f operator-(const Vec3f& lhs, const Vec3f& rhs) { return Vec3f{lhs.s - rhs.s}; }

inline Vec3f operator*(const Vec3f& lhs, const Vec3f& rhs) { return Vec3f{lhs.s * rhs.s}; }

inline Vec3f operator/(const Vec3f& lhs, const Vec3f& rhs) { return Vec3f{lhs.s / rhs.s}; }

inline Vec3f sqrt(const Vec3f& o) { return Vec3f{o.s}; }

} // namespace Effekseer

#endif // __EFFEKSEER_VEC3F_H__