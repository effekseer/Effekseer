#include "GradientHDRState.h"

namespace Effekseer::Tool
{
int GradientHDRState::GetColorCount() const
{
	return state_.ColorCount;
}

int GradientHDRState::GetAlphaCount() const
{
	return state_.AlphaCount;
}

float GradientHDRState::GetColorMarkerPosition(int index) const
{
	return state_.Colors[index].Position;
}

ColorF GradientHDRState::GetColorMarkerColor(int index) const
{
	const auto c = state_.Colors[index].Color;

	return ColorF{
		c[0],
		c[1],
		c[2],
		1.0f,
	};
}

float GradientHDRState::GetColorMarkerIntensity(int index) const
{
	return state_.Colors[index].Intensity;
}

float GradientHDRState::GetAlphaMarkerPosition(int index) const
{
	return state_.Alphas[index].Position;
}

float GradientHDRState::GetAlphaMarkerAlpha(int index) const
{
	return state_.Alphas[index].Alpha;
}

} // namespace Effekseer::Tool