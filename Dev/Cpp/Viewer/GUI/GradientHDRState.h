#pragma once

#include "../Graphics/Color.h"
#include <ImGradientHDR.h>

namespace Effekseer::Tool
{

struct ColorF
{
	float R;
	float G;
	float B;
	float A;

	ColorF()
		: R(0)
		, G(0)
		, B(0)
		, A(0)
	{
	}

	ColorF(float r, float g, float b, float a)
		: R(r)
		, G(g)
		, B(b)
		, A(a)
	{
	}

#if !defined(SWIG)

	bool operator==(const ColorF& o) const
	{
		return R == o.R && G == o.G && B == o.B && A == o.A;
	}

	bool operator!=(const ColorF& o) const
	{
		return !(*this == o);
	}
#endif
};

class GradientHDRState
{
	ImGradientHDRState state_;
	ImGradientHDRTemporaryState temporaryState_;

public:
	int GetColorCount() const;

	int GetAlphaCount() const;

	float GetColorMarkerPosition(int index) const;

	ColorF GetColorMarkerColor(int index) const;

	float GetColorMarkerIntensity(int index) const;

	float GetAlphaMarkerPosition(int index) const;

	float GetAlphaMarkerAlpha(int index) const;

#if !defined(SWIG)
	ImGradientHDRState& GetState()
	{
		return state_;
	}

	ImGradientHDRTemporaryState& GetTemporaryState()
	{
		return temporaryState_;
	}
#endif
};

} // namespace Effekseer::Tool