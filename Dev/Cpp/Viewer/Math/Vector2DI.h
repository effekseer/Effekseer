#pragma once

namespace Effekseer
{
namespace Tool
{

struct Vector2DI
{
	int32_t X;
	int32_t Y;

	Vector2DI()
		: X(0)
		, Y(0)
	{
	}

	Vector2DI(int32_t x, int32_t y)
		: X(x)
		, Y(y)
	{
	}

	bool operator==(const Vector2DI& o) const
	{
		return X == o.X && Y == o.Y;
	}

	bool operator!=(const Vector2DI& o) const
	{
		return X != o.X || Y != o.Y;
	}
};

} // namespace Tool
} // namespace Effekseer