#ifndef __EFFEKSEER_RENDERING_TRANSFORM_H__
#define __EFFEKSEER_RENDERING_TRANSFORM_H__

#include "SIMD/Mat43f.h"

namespace Effekseer
{

/**
	@brief A rendering-only mirror applied to a playing effect.

	The axes are the root-local axes of the effect. This parameter never changes
	the simulation transform or particle state.
*/
struct EffectFlipParameter
{
	bool FlipX = false;
	bool FlipY = false;
	bool FlipZ = false;

	bool operator==(const EffectFlipParameter& rhs) const
	{
		return FlipX == rhs.FlipX && FlipY == rhs.FlipY && FlipZ == rhs.FlipZ;
	}

	bool operator!=(const EffectFlipParameter& rhs) const
	{
		return !(*this == rhs);
	}
};

/**
	@brief Internal transform passed from Manager to renderers.
*/
struct EffectRenderingTransformParameter
{
	SIMD::Mat43f Transform = SIMD::Mat43f::Identity;
	bool IsEnabled = false;
	bool ReversesWinding = false;
};

EffectRenderingTransformParameter CalculateEffectRenderingTransform(
	const SIMD::Mat43f& rootMatrix,
	const EffectFlipParameter& flip);

} // namespace Effekseer

#endif // __EFFEKSEER_RENDERING_TRANSFORM_H__
