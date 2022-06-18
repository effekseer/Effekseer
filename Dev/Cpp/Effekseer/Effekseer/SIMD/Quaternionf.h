
#ifndef __EFFEKSEER_SIMD_QUATERNIONF_H__
#define __EFFEKSEER_SIMD_QUATERNIONF_H__

#include "Float4.h"
#include "Vec3f.h"

namespace Effekseer
{
namespace SIMD
{

struct Quaternionf
{
	Float4 s;

	Quaternionf() = default;

	Quaternionf(float x, float y, float z, float w)
		: s(x, y, z, w)
	{
	}

	Quaternionf(Float4 s)
		: s(s)
	{
	}

	float GetX() const
	{
		return s.GetX();
	}
	float GetY() const
	{
		return s.GetY();
	}
	float GetZ() const
	{
		return s.GetZ();
	}
	float GetW() const
	{
		return s.GetW();
	}

	void SetX(float o)
	{
		s.SetX(o);
	}
	void SetY(float o)
	{
		s.SetY(o);
	}
	void SetZ(float o)
	{
		s.SetZ(o);
	}
	void SetW(float o)
	{
		s.SetW(o);
	}

	Quaternionf Inverse() const
	{
		return Quaternionf{-GetX(), -GetY(), -GetZ(), GetW()};
	}

	static Quaternionf FromMatrix(const Mat44f& mat)
	{
		const auto tr = mat.X.GetX() + mat.Y.GetY() + mat.Z.GetZ();

		if (tr > 0)
		{
			const auto qw = sqrtf(tr + 1.0f) / 2.0f;
			const auto qx = (mat.Z.GetY() - mat.Y.GetZ()) / (4.0f * qw);
			const auto qy = (mat.X.GetZ() - mat.Z.GetX()) / (4.0f * qw);
			const auto qz = (mat.Y.GetX() - mat.X.GetY()) / (4.0f * qw);
			return Quaternionf{qx, qy, qz, qw};
		}
		else if (mat.X.GetX() > mat.Y.GetY() && mat.X.GetX() > mat.Z.GetZ())
		{
			const auto qx = sqrtf(mat.X.GetX() - mat.Y.GetY() - mat.Z.GetZ() + 1.0f) / 2.0f;
			const auto qw = (mat.Z.GetY() - mat.Y.GetZ()) / (4.0f * qx);
			const auto qy = (mat.X.GetY() + mat.Y.GetX()) / (4.0f * qx);
			const auto qz = (mat.X.GetZ() + mat.Z.GetX()) / (4.0f * qx);
			return Quaternionf{qx, qy, qz, qw};
		}
		else if (mat.Y.GetY() > mat.Z.GetZ())
		{
			const auto qy = sqrtf(mat.Y.GetY() - mat.X.GetX() - mat.Z.GetZ() + 1.0f) / 2.0f;
			const auto qw = (mat.X.GetZ() - mat.Z.GetX()) / (4.0f * qy);
			const auto qx = (mat.X.GetY() + mat.Y.GetX()) / (4.0f * qy);
			const auto qz = (mat.Y.GetZ() + mat.Z.GetY()) / (4.0f * qy);
			return Quaternionf{qx, qy, qz, qw};
		}
		else
		{
			const auto qz = sqrtf(mat.Z.GetZ() - mat.X.GetX() - mat.Y.GetY() + 1.0f) / 2.0f;
			const auto qw = (mat.Y.GetX() - mat.X.GetY()) / (4.0f * qz);
			const auto qx = (mat.X.GetZ() + mat.Z.GetX()) / (4.0f * qz);
			const auto qy = (mat.Y.GetZ() + mat.Z.GetY()) / (4.0f * qz);
			return Quaternionf{qx, qy, qz, qw};
		}
	}

	static Quaternionf FromMatrix(const Mat43f& mat)
	{
		const auto tr = mat.X.GetX() + mat.Y.GetY() + mat.Z.GetZ();

		if (tr > 0)
		{
			const auto qw = sqrtf(tr + 1.0f) / 2.0f;
			const auto qx = (mat.Z.GetY() - mat.Y.GetZ()) / (4.0f * qw);
			const auto qy = (mat.X.GetZ() - mat.Z.GetX()) / (4.0f * qw);
			const auto qz = (mat.Y.GetX() - mat.X.GetY()) / (4.0f * qw);
			return Quaternionf{qx, qy, qz, qw};
		}
		else if (mat.X.GetX() > mat.Y.GetY() && mat.X.GetX() > mat.Z.GetZ())
		{
			const auto qx = sqrtf(mat.X.GetX() - mat.Y.GetY() - mat.Z.GetZ() + 1.0f) / 2.0f;
			const auto qw = (mat.Z.GetY() - mat.Y.GetZ()) / (4.0f * qx);
			const auto qy = (mat.X.GetY() + mat.Y.GetX()) / (4.0f * qx);
			const auto qz = (mat.X.GetZ() + mat.Z.GetX()) / (4.0f * qx);
			return Quaternionf{qx, qy, qz, qw};
		}
		else if (mat.Y.GetY() > mat.Z.GetZ())
		{
			const auto qy = sqrtf(mat.Y.GetY() - mat.X.GetX() - mat.Z.GetZ() + 1.0f) / 2.0f;
			const auto qw = (mat.X.GetZ() - mat.Z.GetX()) / (4.0f * qy);
			const auto qx = (mat.X.GetY() + mat.Y.GetX()) / (4.0f * qy);
			const auto qz = (mat.Y.GetZ() + mat.Z.GetY()) / (4.0f * qy);
			return Quaternionf{qx, qy, qz, qw};
		}
		else
		{
			const auto qz = sqrtf(mat.Z.GetZ() - mat.X.GetX() - mat.Y.GetY() + 1.0f) / 2.0f;
			const auto qw = (mat.Y.GetX() - mat.X.GetY()) / (4.0f * qz);
			const auto qx = (mat.X.GetZ() + mat.Z.GetX()) / (4.0f * qz);
			const auto qy = (mat.Y.GetZ() + mat.Z.GetY()) / (4.0f * qz);
			return Quaternionf{qx, qy, qz, qw};
		}
	}

	Mat43f ToMatrix() const
	{
		const auto qx = GetX();
		const auto qy = GetY();
		const auto qz = GetZ();
		const auto qw = GetW();

		const auto qxx = qx * qx;
		const auto qyy = qy * qy;
		const auto qzz = qz * qz;
		const auto qww = qw * qw;

		const auto qxy = qx * qy;
		const auto qxz = qx * qz;
		const auto qyz = qy * qz;

		const auto qxw = qx * qw;
		const auto qyw = qy * qw;
		const auto qzw = qz * qw;

		Mat43f ret;

		ret.X = SIMD::Float4{(qxx - qyy - qzz + qww), 2.0f * (qxy - qzw), 2.0f * (qxz + qyw), 0};
		ret.Y = SIMD::Float4{2.0f * (qxy + qzw), (-qxx + qyy - qzz + qww), 2.0f * (qyz - qxw), 0};
		ret.Z = SIMD::Float4{2.0f * (qxz - qyw), 2.0f * (qyz + qxw), (-qxx - qyy + qzz + qww), 0};

		return ret;
	}

	static Quaternionf Slerp(const Quaternionf& q1, const Quaternionf& q2, float t)
	{
		const auto qq = q1.s * q2.s;
		auto cosa = qq.GetX() + qq.GetY() + qq.GetZ() + qq.GetW();

		if (cosa < 0.0f)
		{
			return Slerp(q1, Quaternionf{-q2.GetX(), -q2.GetY(), -q2.GetZ(), -q2.GetW()}, t);
		}

		cosa = Min(1.0f, cosa);

		const auto alpha = acos(cosa);
		const auto smallValue = 0.00001f;
		if (alpha < smallValue)
		{
			return q1;
		}

		return Quaternionf{q1.s * sin((1.0f - t) * alpha) / sin(alpha) + q2.s * sin(t * alpha) / sin(alpha)};
	}

	static Vec3f Transform(const Vec3f& v, const Quaternionf& q)
	{
		const auto qx = q.GetX();
		const auto qy = q.GetY();
		const auto qz = q.GetZ();
		const auto qw = q.GetW();

		const auto qxx = qx * qx;
		const auto qyy = qy * qy;
		const auto qzz = qz * qz;
		const auto qww = qw * qw;

		const auto qxy = qx * qy;
		const auto qxz = qx * qz;
		const auto qyz = qy * qz;

		const auto qxw = qx * qw;
		const auto qyw = qy * qw;
		const auto qzw = qz * qw;

		const auto x = (qxx - qyy - qzz + qww) * v.GetX() + 2.0f * (qxy - qzw) * v.GetY() + 2.0f * (qxz + qyw) * v.GetZ();
		const auto y = 2.0f * (qxy + qzw) * v.GetX() + (-qxx + qyy - qzz + qww) * v.GetY() + 2.0f * (qyz - qxw) * v.GetZ();
		const auto z = 2.0f * (qxz - qyw) * v.GetX() + 2.0f * (qyz + qxw) * v.GetY() + (-qxx - qyy + qzz + qww) * v.GetZ();

		return Vec3f{x, y, z};
	}
};

inline Quaternionf operator*(const Quaternionf& lhs, const Quaternionf& rhs)
{
	// TODO optimize
	auto x = lhs.GetW() * rhs.GetX() - lhs.GetZ() * rhs.GetY() + lhs.GetY() * rhs.GetZ() + lhs.GetX() * rhs.GetW();
	auto y = lhs.GetZ() * rhs.GetX() + lhs.GetW() * rhs.GetY() - lhs.GetX() * rhs.GetZ() + lhs.GetY() * rhs.GetW();
	auto z = -lhs.GetY() * rhs.GetX() + lhs.GetX() * rhs.GetY() + lhs.GetW() * rhs.GetZ() + lhs.GetZ() * rhs.GetW();
	auto w = -lhs.GetX() * rhs.GetX() - lhs.GetY() * rhs.GetY() - lhs.GetZ() * rhs.GetZ() + lhs.GetW() * rhs.GetW();
	return Quaternionf{x, y, z, w};
}

} // namespace SIMD
} // namespace Effekseer

#endif