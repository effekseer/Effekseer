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

enum class GradientHDRMarkerType : int32_t
{
	Color,
	Alpha,
	Unknown,
};

class GradientHDRState
{
	ImGradientHDRState state_;

public:
	int GetColorCount() const;

	void SetColorCount(int value);

	int GetAlphaCount() const;

	void SetAlphaCount(int value);

	float GetColorMarkerPosition(int index) const;

	void SetColorMarkerPosition(int index, float value);

	ColorF GetColorMarkerColor(int index) const;

	void SetColorMarkerColor(int index, ColorF value);

	float GetColorMarkerIntensity(int index) const;

	void SetColorMarkerIntensity(int index, float value);

	float GetAlphaMarkerPosition(int index) const;

	void SetAlphaMarkerPosition(int index, float value);

	float GetAlphaMarkerAlpha(int index) const;

	void SetAlphaMarkerAlpha(int index, float value);

	bool RemoveColorMarker(int32_t index);

	bool RemoveAlphaMarker(int32_t index);

#if !defined(SWIG)
	ImGradientHDRState& GetState()
	{
		return state_;
	}
#endif
};

class GradientHDRGUIState
{
	ImGradientHDRTemporaryState temporaryState_;

public:
	GradientHDRMarkerType GetSelectedMarkerType() const;

	int32_t GetSelectedIndex() const;

#if !defined(SWIG)
	ImGradientHDRTemporaryState& GetTemporaryState()
	{
		return temporaryState_;
	}
#endif
};

} // namespace Effekseer::Tool