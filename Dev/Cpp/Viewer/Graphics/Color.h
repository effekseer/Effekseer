#pragma once

#include <Effekseer.h>
#include <stdint.h>

namespace Effekseer::Tool
{

struct Color
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
	uint8_t A;

	Color()
		: R(0)
		, G(0)
		, B(0)
		, A(0)
	{
	}

	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
		: R(r)
		, G(g)
		, B(b)
		, A(a)
	{
	}

#if !defined(SWIG)

	bool operator==(const Color& o) const
	{
		return R == o.R && G == o.G && B == o.B && A == o.A;
	}

	bool operator!=(const Color& o) const
	{
		return !(*this == o);
	}

	Color(const Effekseer::Color& v)
		: Color(v.R, v.G, v.B, v.A)
	{
	}

	operator Effekseer::Color() const
	{
		return Effekseer::Color{
			R, G, B, A};
	}
#endif
};

} // namespace Effekseer::Tool