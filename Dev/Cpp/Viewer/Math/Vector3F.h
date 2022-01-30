#pragma once

#include <Effekseer.h>
#include <stdint.h>

namespace Effekseer
{
namespace Tool
{

struct Vector3F
{
	float X;
	float Y;
	float Z;

	Vector3F()
		: X(0)
		, Y(0)
		, Z(0)
	{
	}

	Vector3F(float x, float y, float z)
		: X(x)
		, Y(y)
		, Z(z)
	{
	}

	Vector3F operator+(const Vector3F& o) const
	{
		return Vector3F(X + o.X, Y + o.Y, Z + o.Z);
	}

#if !defined(SWIG)

	Vector3F(const Effekseer::Vector3D& v)
		: X(v.X)
		, Y(v.Y)
		, Z(v.Z)
	{
	}

	operator Effekseer::Vector3D() const
	{
		return Vector3D{X, Y, Z};
	}
#endif
};

} // namespace Tool
} // namespace Effekseer