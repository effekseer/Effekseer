
#ifndef __EFFEKSEER_VECTOR2D_H__
#define __EFFEKSEER_VECTOR2D_H__

#include "Effekseer.Base.Pre.h"

namespace Effekseer
{

/**
	@brief
	\~English 2D Vector
	\~Japanese 2次元ベクトル
*/
struct Vector2D
{
public:
	float X = 0.0f;

	float Y = 0.0f;

	Vector2D() = default;

	Vector2D(float x, float y): X(x), Y(y) {}

	Vector2D& operator+=(const Vector2D& value);

	Vector2D& operator-=(const Vector2D& value);

	/**
		@brief 
		\~English	Convert Vector2D into std::array<float,2>
		\~Japanese	Vector2D から std::array<float,2> に変換する。
	*/
	std::array<float, 2> ToFloat2() const
	{
		return {X, Y};
	}
};

} // namespace Effekseer

#endif // __EFFEKSEER_VECTOR2D_H__
