
#include "Effekseer.RectF.h"
#include "Effekseer.Vector2D.h"

namespace Effekseer
{


Vector2D RectF::Position() const
{
	return Vector2D(X, Y);
}

Vector2D RectF::Size() const
{
	return Vector2D(Width, Height);
}

} // namespace Effekseer
