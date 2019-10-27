
#ifndef __EFFEKSEER_VEC4FBLOCK4_H__
#define __EFFEKSEER_VEC4FBLOCK4_H__

#include "Effekseer.Mat44f.h"
#include "Effekseer.Vec3f.h"
#include "Effekseer.Vec4f.h"

namespace Effekseer
{

/**
	@brief	Operate Vec4 simultaneously
*/
struct Vec4fBlock4
{
	Vec4f X;
	Vec4f Y;
	Vec4f Z;
	Vec4f W;

	void Get(Vec4f& v1, Vec4f& v2, Vec4f& v3, Vec4f& v4)
	{

		SIMD4f x, y, z, w;
		transpose(x, y, z, w, X.s, Y.s, Z.s, W.s);

		v1.s = x;
		v2.s = y;
		v3.s = z;
		v4.s = w;
	}

	void Set(const Vec4f& v1, const Vec4f& v2, const Vec4f& v3, const Vec4f& v4)
	{
		SIMD4f x, y, z, w;
		transpose(x, y, z, w, v1, v2, v3, v4);

		X.s = x;
		Y.s = y;
		Z.s = z;
		W.s = w;
	}

	Vec4fBlock4 operator+(const Vec4fBlock4& o) const
	{
		return Vec4fBlock4{
			this->X + o.X,
			this->Y + o.Y,
			this->Z + o.Z,
			this->W + o.W,
		};
	}

	Vec4fBlock4 operator-(const Vec4fBlock4& o) const
	{
		return Vec4fBlock4{
			this->X - o.X,
			this->Y - o.Y,
			this->Z - o.Z,
			this->W - o.W,
		};
	}

	Vec4fBlock4 operator*(const Vec4fBlock4& o) const
	{
		return Vec4fBlock4{
			this->X * o.X,
			this->Y * o.Y,
			this->Z * o.Z,
			this->W * o.W,
		};
	}

	Vec4fBlock4 operator/(const Vec4fBlock4& o) const
	{
		return Vec4fBlock4{
			this->X / o.X,
			this->Y / o.Y,
			this->Z / o.Z,
			this->W / o.W,
		};
	}

	static Vec4f Dot(const Vec4fBlock4& lhs, const Vec4fBlock4& rhs)
	{
		return sqrt(lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z + lhs.W * rhs.W);
	}
};

inline Vec4fBlock4 sqrt(const Vec4fBlock4& o)
{
	return Vec4fBlock4{
		sqrt(o.X),
		sqrt(o.Y),
		sqrt(o.Z),
		sqrt(o.W),
	};
}

} // namespace Effekseer

#endif // __EFFEKSEER_Vec4fBlock4_H__