#include "Effekseer.RenderingTransform.h"
#include "Effekseer.Matrix44.h"
#include "SIMD/Mat44f.h"
#include "SIMD/Utils.h"
#include <cmath>

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

bool IsValidRenderingCoordinateMatrix(const Matrix44& matrix, float epsilon)
{
	for (int32_t row = 0; row < 4; row++)
	{
		for (int32_t column = 0; column < 4; column++)
		{
			if (!std::isfinite(matrix.Values[row][column]))
			{
				return false;
			}
		}
	}

	if (std::abs(matrix.Values[0][3]) > epsilon ||
		std::abs(matrix.Values[1][3]) > epsilon ||
		std::abs(matrix.Values[2][3]) > epsilon ||
		std::abs(matrix.Values[3][0]) > epsilon ||
		std::abs(matrix.Values[3][1]) > epsilon ||
		std::abs(matrix.Values[3][2]) > epsilon ||
		std::abs(matrix.Values[3][3] - 1.0f) > epsilon)
	{
		return false;
	}

	for (int32_t row = 0; row < 3; row++)
	{
		for (int32_t other = row; other < 3; other++)
		{
			float dot = 0.0f;
			for (int32_t column = 0; column < 3; column++)
			{
				dot += matrix.Values[row][column] * matrix.Values[other][column];
			}

			const float expected = row == other ? 1.0f : 0.0f;
			if (std::abs(dot - expected) > epsilon)
			{
				return false;
			}
		}
	}

	const float determinant =
		matrix.Values[0][0] * (matrix.Values[1][1] * matrix.Values[2][2] - matrix.Values[1][2] * matrix.Values[2][1]) -
		matrix.Values[0][1] * (matrix.Values[1][0] * matrix.Values[2][2] - matrix.Values[1][2] * matrix.Values[2][0]) +
		matrix.Values[0][2] * (matrix.Values[1][0] * matrix.Values[2][1] - matrix.Values[1][1] * matrix.Values[2][0]);
	return std::abs(std::abs(determinant) - 1.0f) <= epsilon;
}

EffectRenderingTransformParameter CalculateRenderingCoordinateTransform(const Matrix44& matrix)
{
	EFK_ASSERT(IsValidRenderingCoordinateMatrix(matrix));

	EffectRenderingTransformParameter result;
	result.Transform = ToMat43f(matrix);
	result.IsEnabled = !SIMD::Mat43f::Equal(result.Transform, SIMD::Mat43f::Identity);

	const float determinant =
		matrix.Values[0][0] * (matrix.Values[1][1] * matrix.Values[2][2] - matrix.Values[1][2] * matrix.Values[2][1]) -
		matrix.Values[0][1] * (matrix.Values[1][0] * matrix.Values[2][2] - matrix.Values[1][2] * matrix.Values[2][0]) +
		matrix.Values[0][2] * (matrix.Values[1][0] * matrix.Values[2][1] - matrix.Values[1][1] * matrix.Values[2][0]);
	result.ReversesWinding = determinant < 0.0f;
	return result;
}

EffectRenderingTransformParameter ComposeRenderingTransforms(
	const EffectRenderingTransformParameter& first,
	const EffectRenderingTransformParameter& second)
{
	if (!first.IsEnabled)
	{
		return second;
	}
	if (!second.IsEnabled)
	{
		return first;
	}

	EffectRenderingTransformParameter result;
	result.Transform = first.Transform * second.Transform;
	result.IsEnabled = true;
	result.ReversesWinding = first.ReversesWinding ^ second.ReversesWinding;
	return result;
}

} // namespace Effekseer
