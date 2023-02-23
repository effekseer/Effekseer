
#pragma once

namespace efk
{
struct Vec4
{
	float X;
	float Y;
	float Z;
	float W;

	Vec4()
	{
		X = 0;
		Y = 0;
		Z = 0;
		W = 1;
	}

	Vec4(float x, float y, float z, float w)
	{
		X = x;
		Y = y;
		Z = z;
		W = w;
	}
};
} // namespace efk
