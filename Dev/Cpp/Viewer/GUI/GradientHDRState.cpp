#include "GradientHDRState.h"

namespace Effekseer::Tool
{
int GradientHDRState::GetColorCount() const
{
	return state_.ColorCount;
}

void GradientHDRState::SetColorCount(int value)
{
	state_.ColorCount = value;
}

int GradientHDRState::GetAlphaCount() const
{
	return state_.AlphaCount;
}

void GradientHDRState::SetAlphaCount(int value)
{
	state_.AlphaCount = value;
}

float GradientHDRState::GetColorMarkerPosition(int index) const
{
	return state_.Colors[index].Position;
}

void GradientHDRState::SetColorMarkerPosition(int index, float value)
{
	state_.Colors[index].Position = value;
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

void GradientHDRState::SetColorMarkerColor(int index, ColorF value)
{
	state_.Colors[index].Color[0] = value.R;
	state_.Colors[index].Color[1] = value.G;
	state_.Colors[index].Color[2] = value.B;
}

float GradientHDRState::GetColorMarkerIntensity(int index) const
{
	return state_.Colors[index].Intensity;
}

void GradientHDRState::SetColorMarkerIntensity(int index, float value)
{
	state_.Colors[index].Intensity = value;
}

float GradientHDRState::GetAlphaMarkerPosition(int index) const
{
	return state_.Alphas[index].Position;
}

void GradientHDRState::SetAlphaMarkerPosition(int index, float value)
{
	state_.Alphas[index].Position = value;
}

float GradientHDRState::GetAlphaMarkerAlpha(int index) const
{
	return state_.Alphas[index].Alpha;
}

void GradientHDRState::SetAlphaMarkerAlpha(int index, float value)
{
	state_.Alphas[index].Alpha = value;
}

bool GradientHDRState::RemoveColorMarker(int32_t index)
{
	return state_.RemoveColorMarker(index);
}

bool GradientHDRState::RemoveAlphaMarker(int32_t index)
{
	return state_.RemoveAlphaMarker(index);
}

GradientHDRMarkerType GradientHDRGUIState::GetSelectedMarkerType() const
{
	return static_cast<GradientHDRMarkerType>(temporaryState_.selectedMarkerType);
}

int32_t GradientHDRGUIState::GetSelectedIndex() const
{
	return temporaryState_.selectedIndex;
}

} // namespace Effekseer::Tool