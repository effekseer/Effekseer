
#ifndef __EFFEKSEER_RECTF_H__
#define __EFFEKSEER_RECTF_H__

#include <stdint.h>

namespace Effekseer
{

struct Vector2D;

/**
	@brief
	\~English Rectangle
	\~Japanese	矩形
*/
struct RectF
{
private:
public:
	float X = 0.0f;

	float Y = 0.0f;

	float Width = 1.0f;

	float Height = 1.0f;

	RectF() = default;

	RectF(float x, float y, float width, float height)
		: X(x)
		, Y(y)
		, Width(width)
		, Height(height)
	{
	}

	/**
		@brief
		\~English	Get position
		\~Japanese	位置を取得する
	*/
	Vector2D Position() const;

	/**
		@brief
		\~English	Get size
		\~Japanese	サイズを取得する
	*/
	Vector2D Size() const;
};

} // namespace Effekseer

#endif // __EFFEKSEER_RECTF_H__
