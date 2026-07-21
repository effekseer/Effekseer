#ifndef __EFFEKSEER_RENDERING_TRANSFORM_H__
#define __EFFEKSEER_RENDERING_TRANSFORM_H__

#include "Effekseer.Matrix44.h"
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
	// Geometric parity used for coordinate-system, normal, and tangent handling.
	bool ReversesWinding = false;
	// Whether the paired camera matrix uses the opposite handedness convention.
	// Unlike ReversesWinding, this is not changed by an effect-only reflection.
	bool ReversesCameraFront = false;
	// Whether renderers must exchange front- and back-face culling. A coordinate
	// boundary paired with an external camera reverses geometric parity without
	// reversing the projected face convention.
	bool ReversesCulling = false;
};

EffectRenderingTransformParameter CalculateEffectRenderingTransform(
	const SIMD::Mat43f& rootMatrix,
	const EffectFlipParameter& flip);

/**
	@brief Returns whether a matrix can be used as a rendering coordinate transform.

	A rendering coordinate transform is restricted to an orthogonal axis transform.
	It may exchange or reflect axes, but must not contain translation, scale, shear,
	or perspective.
*/
bool IsValidRenderingCoordinateMatrix(const Matrix44& matrix, float epsilon = 0.0001f);

/**
	@brief Converts a draw-path coordinate matrix to the internal renderer parameter.
	@note The matrix must satisfy IsValidRenderingCoordinateMatrix().
*/
EffectRenderingTransformParameter CalculateRenderingCoordinateTransform(const Matrix44& matrix);

/**
	@brief Composes rendering transforms in row-vector application order.
*/
EffectRenderingTransformParameter ComposeRenderingTransforms(
	const EffectRenderingTransformParameter& first,
	const EffectRenderingTransformParameter& second);

/**
	@brief Applies the renderer-side face-culling parity of a rendering transform.
*/
CullingType GetTransformedCullingType(
	CullingType cullingType,
	const EffectRenderingTransformParameter& transform);

} // namespace Effekseer

#endif // __EFFEKSEER_RENDERING_TRANSFORM_H__
