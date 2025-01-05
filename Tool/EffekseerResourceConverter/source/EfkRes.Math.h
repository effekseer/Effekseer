#pragma once

#include <math.h>
#include <array>

namespace efkres
{

struct Vec2
{
	union
	{
		struct
		{
			double x, y;
		};
		double v[2];
	};

	Vec2() = default;
	Vec2(double x, double y)
		: x(x), y(y)
	{
	}
};
inline bool operator==(Vec2 lhs, Vec2 rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}
inline Vec2 operator+(Vec2 lhs, Vec2 rhs)
{
	return Vec2{ lhs.x + rhs.x, lhs.y + rhs.y };
}
inline Vec2 operator-(Vec2 lhs, Vec2 rhs)
{
	return Vec2{ lhs.x - rhs.x, lhs.y - rhs.y };
}
inline Vec2 operator*(Vec2 lhs, double rhs)
{
	return Vec2{ lhs.x * rhs, lhs.y * rhs };
}
inline Vec2 operator/(Vec2 lhs, double rhs)
{
	return Vec2{ lhs.x / rhs, lhs.y / rhs };
}

struct Vec3
{
	union
	{
		struct
		{
			double x, y, z;
		};
		double v[3];
	};

	Vec3() = default;
	Vec3(double x, double y, double z)
		: x(x), y(y), z(z)
	{
	}

	double Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	Vec3 Normalized() const
	{
		double length = Length();
		return { x / length, y / length, z / length };
	}

	Vec3& operator+=(Vec3 rhs);
	Vec3& operator-=(Vec3 rhs);
	Vec3& operator*=(double rhs);
	Vec3& operator/=(double rhs);
};
inline bool operator==(Vec3 lhs, Vec3 rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}
inline Vec3 operator+(Vec3 lhs, Vec3 rhs)
{
	return Vec3{ lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}
inline Vec3 operator-(Vec3 lhs, Vec3 rhs)
{
	return Vec3{ lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}
inline Vec3 operator*(Vec3 lhs, double rhs)
{
	return Vec3{ lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
}
inline Vec3 operator/(Vec3 lhs, double rhs)
{
	return Vec3{ lhs.x / rhs, lhs.y / rhs, lhs.z / rhs };
}
inline double DotProduct(Vec3 lhs, Vec3 rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}
inline Vec3 CrossProduct(Vec3 lhs, Vec3 rhs)
{
	return Vec3{
		lhs.y * rhs.z - lhs.z * rhs.y,
		lhs.z * rhs.x - lhs.x * rhs.z,
		lhs.x * rhs.y - lhs.y * rhs.x,
	};
}
inline Vec3& Vec3::operator+=(Vec3 rhs)
{
	return *this = *this + rhs;
}
inline Vec3& Vec3::operator-=(Vec3 rhs)
{
	return *this = *this - rhs;
}
inline Vec3& Vec3::operator*=(double rhs)
{
	return *this = *this * rhs;
}
inline Vec3& Vec3::operator/=(double rhs)
{
	return *this = *this / rhs;
}

struct Vec4
{
	union
	{
		struct
		{
			double x, y, z, w;
		};
		double v[4];
	};

	Vec4() = default;
	Vec4(double x, double y, double z, double w)
		: x(x), y(y), z(z), w(w)
	{
	}
};
inline bool operator==(Vec4 lhs, Vec4 rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
}


struct Mat43
{
	union
	{
		struct
		{
			double m00, m10, m20;
			double m01, m11, m21;
			double m02, m12, m22;
			double m03, m13, m23;
		};
		Vec3 cols[4];
		double v[12];
	};

	Mat43() = default;
	Mat43(Vec3 col0, Vec3 col1, Vec3 col2, Vec3 col3)
	{
		cols[0] = col0;
		cols[1] = col1;
		cols[2] = col2;
		cols[3] = col3;
	}

	Vec3 TransformPosition(Vec3 v) const
	{
		return cols[0] * v.x + cols[1] * v.y + cols[2] * v.z + cols[3];
	}

	Vec3 TransformDirection(Vec3 v) const
	{
		return cols[0] * v.x + cols[1] * v.y + cols[2] * v.z;
	}

	static Mat43 Identity()
	{
		return Mat43(
			Vec3(1.0f, 0.0f, 0.0f),
			Vec3(0.0f, 1.0f, 0.0f),
			Vec3(0.0f, 0.0f, 1.0f),
			Vec3(0.0f, 0.0f, 0.0f)
		);
	}
};

struct Quat
{
	double x, y, z, w;

	Quat() = default;
	Quat(double w, double x, double y, double z)
		: w(w), x(x), y(y), z(z)
	{
	}

	Quat operator-() { return Quat(-w, -x, -y, -z); }

	void SetTBN(Vec3 col0, Vec3 col1, Vec3 col2);
};

inline void Quat::SetTBN(Vec3 col0, Vec3 col1, Vec3 col2)
{
	double px = col0.x - col1.y - col2.z;
	double py = col1.y - col0.x - col2.z;
	double pz = col2.z - col0.x - col1.y;
	double pw = col0.x + col1.y + col2.z;

	int selected = 0;
	double pmax = pw;
	if (px > pmax)
	{
		pmax = px;
		selected = 1;
	}
	if (py > pmax)
	{
		pmax = py;
		selected = 2;
	}
	if (pz > pmax)
	{
		pmax = pz;
		selected = 3;
	}

	double maxVal = sqrt(pmax + 1.0) * 0.5;
	double mult = 0.25 / maxVal;

	switch(selected)
	{
	case 0:
		*this = Quat(maxVal, (col1.z - col2.y) * mult, (col2.x - col0.z) * mult, (col0.y - col1.x) * mult);
		break;
	case 1:
		*this = Quat((col1.z - col2.y) * mult, maxVal, (col0.y + col1.x) * mult, (col2.x + col0.z) * mult);
		break;
	case 2:
		*this = Quat((col2.x - col0.z) * mult, (col0.y + col1.x) * mult, maxVal, (col1.z + col2.y) * mult);
		break;
	case 3:
		*this = Quat((col0.y - col1.x) * mult, (col2.x + col0.z) * mult, (col1.z + col2.y) * mult, maxVal);
		break;
	default:
		*this = Quat(1, 0, 0, 0);
		break;
	}
}

inline Vec3 xAxis(Quat qQuat)
{
	double fTy = 2.0 * qQuat.y;
	double fTz = 2.0 * qQuat.z;
	double fTwy = fTy * qQuat.w;
	double fTwz = fTz * qQuat.w;
	double fTxy = fTy * qQuat.x;
	double fTxz = fTz * qQuat.x;
	double fTyy = fTy * qQuat.y;
	double fTzz = fTz * qQuat.z;

	return Vec3(1.0 - (fTyy + fTzz), fTxy + fTwz, fTxz - fTwy);
}

inline Vec3 yAxis(Quat qQuat)
{
	double fTx = 2.0 * qQuat.x;
	double fTy = 2.0 * qQuat.y;
	double fTz = 2.0 * qQuat.z;
	double fTwx = fTx * qQuat.w;
	double fTwz = fTz * qQuat.w;
	double fTxx = fTx * qQuat.x;
	double fTxy = fTy * qQuat.x;
	double fTyz = fTz * qQuat.y;
	double fTzz = fTz * qQuat.z;

	return Vec3(fTxy - fTwz, 1.0 - (fTxx + fTzz), fTyz + fTwx);
}

}
