#include "Effekseer.SIMDUtils.h"
#include "../Effekseer.InternalStruct.h"
#include "../Effekseer.Vector2D.h"
#include "../Effekseer.Vector3D.h"
#include "Effekseer.Vec2f.h"
#include "Effekseer.Vec3f.h"
#include "Effekseer.Vec4f.h"

namespace Effekseer
{

//----------------------------------------------------------------------------------
// Temporary implementation
//----------------------------------------------------------------------------------
Vec2f::Vec2f(const vector2d& vec)
	: s(vec.x, vec.y, 0.0f, 0.0f)
{
}

Vec2f::Vec2f(const Vector2D& vec)
	: s(vec.X, vec.Y, 0.0f, 0.0f)
{
}

Vec3f::Vec3f(const vector3d& vec)
	: s(vec.x, vec.y, vec.z, 0.0f)
{
}

Vec3f::Vec3f(const Vector3D& vec)
	: s(vec.X, vec.Y, vec.Z, 0.0f)
{
}

} // namespace Effekseer
