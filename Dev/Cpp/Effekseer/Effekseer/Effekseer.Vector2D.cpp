
#include "Effekseer.Vector2D.h"

namespace Effekseer
{

Vector2D& Vector2D::operator+=(const Vector2D& value)
{
	X += value.X;
	Y += value.Y;
	return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& value)
{
	X -= value.X;
	Y -= value.Y;
	return *this;
}

} // namespace Effekseer
