#include "Effekseer.RenderingTransform.h"
#include "Effekseer.Matrix44.h"
#include "SIMD/Mat44f.h"
#include "SIMD/Utils.h"

namespace Effekseer
{

namespace
{

SIMD::Mat43f ToMat43f(const Matrix44& mat)
{
	return SIMD::Mat43f(mat.Values[0][0],
						mat.Values[0][1],
						mat.Values[0][2],
						mat.Values[1][0],
						mat.Values[1][1],
						mat.Values[1][2],
						mat.Values[2][0],
						mat.Values[2][1],
						mat.Values[2][2],
						mat.Values[3][0],
						mat.Values[3][1],
						mat.Values[3][2]);
}

} // namespace

EffectRenderingTransformParameter CalculateEffectRenderingTransform(
	const SIMD::Mat43f& rootMatrix,
	const EffectFlipParameter& flip)
{
	EffectRenderingTransformParameter result;
	result.IsEnabled = flip.FlipX || flip.FlipY || flip.FlipZ;
	result.ReversesWinding = flip.FlipX ^ flip.FlipY ^ flip.FlipZ;

	if (!result.IsEnabled)
	{
		return result;
	}

	Matrix44 inverseRoot;
	Matrix44::Inverse(inverseRoot, ToStruct(SIMD::Mat44f(rootMatrix)));
	const auto flipMatrix = SIMD::Mat43f::Scaling(
		flip.FlipX ? -1.0f : 1.0f,
		flip.FlipY ? -1.0f : 1.0f,
		flip.FlipZ ? -1.0f : 1.0f);
	result.Transform = ToMat43f(inverseRoot) * flipMatrix * rootMatrix;
	return result;
}

} // namespace Effekseer
