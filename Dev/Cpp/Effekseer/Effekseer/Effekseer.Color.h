
#ifndef __EFFEKSEER_COLOR_H__
#define __EFFEKSEER_COLOR_H__

#include <array>
#include <stdint.h>

namespace Effekseer
{

/**
	@brief
	\~English Color space mode
	\~Japanese	色空間モード
*/
enum ColorMode : uint32_t
{
	COLOR_MODE_RGBA, // RGB + α
	COLOR_MODE_HSVA, // HSV + α
};

/**
	@brief	8bit UNorm RGBA color
*/
struct Color
{
	/**
		@brief
		\~English	Red
		\~Japanese	赤
	*/
	uint8_t R;

	/**
		@brief
		\~English	Green
		\~Japanese	緑
	*/
	uint8_t G;

	/**
		@brief
		\~English	Blue
		\~Japanese	青
	*/
	uint8_t B;

	/**
		@brief
		\~English	Alpha
		\~Japanese	透明度
	*/
	uint8_t A;

	Color() = default;

	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

	/**
		@brief
		\~English	Convert Color into std::array<float,4>
		\~Japanese	Color から std::array<float,4> に変換する。
	*/
	std::array<float, 4> ToFloat4() const
	{
		return {
			static_cast<float>(R) / 255.0f,
			static_cast<float>(G) / 255.0f,
			static_cast<float>(B) / 255.0f,
			static_cast<float>(A) / 255.0f};
	}

	/**
		@brief
		\~English	Multiply colors
		\~Japanese	色の乗算
	*/
	static Color Mul(Color in1, Color in2);
	static Color Mul(Color in1, float in2);

	/**
		@brief
		\~English	Linearly interpolate between colors
		\~Japanese	色の線形補間
	*/
	static Color Lerp(const Color in1, const Color in2, float t);

	bool operator==(const Color& o) const
	{
		return R == o.R && G == o.G && B == o.B && A == o.A;
	}

	bool operator!=(const Color& o) const
	{
		return R != o.R || G != o.G || B != o.B || A != o.A;
	}

	bool operator<(const Color& o) const
	{
		if (R != o.R)
			return R < o.R;

		if (G != o.G)
			return G < o.G;

		if (B != o.B)
			return B < o.B;

		if (A != o.A)
			return A < o.A;

		return false;
	}
};

} // namespace Effekseer

#endif // __EFFEKSEER_COLOR_H__
