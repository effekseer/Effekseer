
#ifndef __EFFEKSEER_VEC3FBLOCK4_H__
#define __EFFEKSEER_VEC3FBLOCK4_H__

#include "Effekseer.Mat44f.h"
#include "Effekseer.Vec3f.h"
#include "Effekseer.Vec4f.h"

namespace Effekseer
{

/**
	@brief	Operate Vec3 simultaneously
*/
struct Vec3fBlock4
{
	Vec4f X;
	Vec4f Y;
	Vec4f Z;

	void Get(Vec3f& v1, Vec3f& v2, Vec3f& v3, Vec3f& v4)
	{

		SIMD4f x, y, z, w, t;
		transpose(x, y, z, w, X.s, Y.s, Z.s, t);

		v1.s = x;
		v2.s = y;
		v3.s = z;
		v4.s = w;
	}

	void Set(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, const Vec3f& v4)
	{
		SIMD4f x, y, z, w;
		transpose(x, y, z, w, v1.s, v2.s, v3.s, v4.s);

		X.s = x;
		Y.s = y;
		Z.s = z;
	}

	Vec3fBlock4 operator+(const Vec3fBlock4& o) const
	{
		return Vec3fBlock4{
			this->X + o.X,
			this->Y + o.Y,
			this->Z + o.Z,
		};
	}

	Vec3fBlock4 operator-(const Vec3fBlock4& o) const
	{
		return Vec3fBlock4{
			this->X - o.X,
			this->Y - o.Y,
			this->Z - o.Z,
		};
	}

	Vec3fBlock4 operator*(const Vec3fBlock4& o) const
	{
		return Vec3fBlock4{
			this->X * o.X,
			this->Y * o.Y,
			this->Z * o.Z,
		};
	}

	Vec3fBlock4 operator/(const Vec3fBlock4& o) const
	{
		return Vec3fBlock4{
			this->X / o.X,
			this->Y / o.Y,
			this->Z / o.Z,
		};
	}

	static Vec4f Dot(const Vec3fBlock4& lhs, const Vec3fBlock4& rhs) { return sqrt(lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z); }

	static Vec3fBlock4& Cross(Vec3fBlock4& o, const Vec3fBlock4& in1, const Vec3fBlock4& in2)
	{
		auto x = in1.Y * in2.Z - in1.Z * in2.Y;
		auto y = in1.Z * in2.X - in1.X * in2.Z;
		auto z = in1.X * in2.Y - in1.Y * in2.X;
		o.X = x;
		o.Y = y;
		o.Z = z;
		return o;
	}
};

inline Vec3fBlock4 sqrt(const Vec3fBlock4& o)
{
	return Vec3fBlock4{
		sqrt(o.X),
		sqrt(o.Y),
		sqrt(o.Z),
	};
}

} // namespace Effekseer

#endif // __EFFEKSEER_VEC3FBLOCK4_H__