#include "Effekseer.RenderingTransform.h"
#include "Effekseer.Matrix44.h"
#include "SIMD/Mat44f.h"
#include "SIMD/Utils.h"
#include <algorithm>
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

SIMD::Mat43f ExtractRigidRoot(const SIMD::Mat43f& rootMatrix)
{
	const auto root = ToStruct(rootMatrix);
	if (root.IsProperSRT())
	{
		SIMD::Vec3f scale;
		SIMD::Mat43f rotation;
		SIMD::Vec3f translation;
		rootMatrix.GetSRT(scale, rotation, translation);
		rotation.SetTranslation(translation);
		return rotation;
	}

	double rotation[3][3];
	for (int32_t row = 0; row < 3; row++)
	{
		for (int32_t column = 0; column < 3; column++)
		{
			rotation[row][column] = root.Value[row][column];
		}
	}

	// A product of individually valid SRT matrices can contain shear. Use the
	// orthogonal factor of the polar decomposition so an emitter flip remains a
	// true reflection and all renderer-side direction transforms stay valid.
	for (int32_t iteration = 0; iteration < 16; iteration++)
	{
		const double determinant =
			rotation[0][0] * (rotation[1][1] * rotation[2][2] - rotation[1][2] * rotation[2][1]) -
			rotation[0][1] * (rotation[1][0] * rotation[2][2] - rotation[1][2] * rotation[2][0]) +
			rotation[0][2] * (rotation[1][0] * rotation[2][1] - rotation[1][1] * rotation[2][0]);
		if (!std::isfinite(determinant) || std::abs(determinant) < 1.0e-12)
		{
			return SIMD::Mat43f::Translation(root.Value[3][0], root.Value[3][1], root.Value[3][2]);
		}

		double inverseTranspose[3][3];
		inverseTranspose[0][0] = (rotation[1][1] * rotation[2][2] - rotation[1][2] * rotation[2][1]) / determinant;
		inverseTranspose[0][1] = (rotation[1][2] * rotation[2][0] - rotation[1][0] * rotation[2][2]) / determinant;
		inverseTranspose[0][2] = (rotation[1][0] * rotation[2][1] - rotation[1][1] * rotation[2][0]) / determinant;
		inverseTranspose[1][0] = (rotation[0][2] * rotation[2][1] - rotation[0][1] * rotation[2][2]) / determinant;
		inverseTranspose[1][1] = (rotation[0][0] * rotation[2][2] - rotation[0][2] * rotation[2][0]) / determinant;
		inverseTranspose[1][2] = (rotation[0][1] * rotation[2][0] - rotation[0][0] * rotation[2][1]) / determinant;
		inverseTranspose[2][0] = (rotation[0][1] * rotation[1][2] - rotation[0][2] * rotation[1][1]) / determinant;
		inverseTranspose[2][1] = (rotation[0][2] * rotation[1][0] - rotation[0][0] * rotation[1][2]) / determinant;
		inverseTranspose[2][2] = (rotation[0][0] * rotation[1][1] - rotation[0][1] * rotation[1][0]) / determinant;

		double maximumDifference = 0.0;
		for (int32_t row = 0; row < 3; row++)
		{
			for (int32_t column = 0; column < 3; column++)
			{
				const double next = 0.5 * (rotation[row][column] + inverseTranspose[row][column]);
				maximumDifference = std::max(maximumDifference, std::abs(next - rotation[row][column]));
				rotation[row][column] = next;
			}
		}
		if (maximumDifference < 1.0e-7)
		{
			break;
		}
	}

	return SIMD::Mat43f(
		static_cast<float>(rotation[0][0]), static_cast<float>(rotation[0][1]), static_cast<float>(rotation[0][2]),
		static_cast<float>(rotation[1][0]), static_cast<float>(rotation[1][1]), static_cast<float>(rotation[1][2]),
		static_cast<float>(rotation[2][0]), static_cast<float>(rotation[2][1]), static_cast<float>(rotation[2][2]),
		root.Value[3][0], root.Value[3][1], root.Value[3][2]);
}

} // namespace

EffectRenderingTransformParameter CalculateEffectRenderingTransform(
	const SIMD::Mat43f& rootMatrix,
	const EffectFlipParameter& flip)
{
	EffectRenderingTransformParameter result;
	result.IsEnabled = flip.FlipX || flip.FlipY || flip.FlipZ;
	result.ReversesWinding = flip.FlipX ^ flip.FlipY ^ flip.FlipZ;
	result.ReversesCulling = result.ReversesWinding;

	if (!result.IsEnabled)
	{
		return result;
	}

	const auto rigidRoot = ExtractRigidRoot(rootMatrix);
	Matrix44 inverseRoot;
	Matrix44::Inverse(inverseRoot, ToStruct(SIMD::Mat44f(rigidRoot)));
	const auto flipMatrix = SIMD::Mat43f::Scaling(
		flip.FlipX ? -1.0f : 1.0f,
		flip.FlipY ? -1.0f : 1.0f,
		flip.FlipZ ? -1.0f : 1.0f);
	result.Transform = ToMat43f(inverseRoot) * flipMatrix * rigidRoot;
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
	result.ReversesCulling = result.ReversesWinding;
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
	result.ReversesCameraFront = first.ReversesCameraFront ^ second.ReversesCameraFront;
	result.ReversesCulling = first.ReversesCulling ^ second.ReversesCulling;
	return result;
}

CullingType GetTransformedCullingType(
	CullingType cullingType,
	const EffectRenderingTransformParameter& transform)
{
	if (!transform.ReversesCulling)
	{
		return cullingType;
	}

	if (cullingType == CullingType::Front)
	{
		return CullingType::Back;
	}
	if (cullingType == CullingType::Back)
	{
		return CullingType::Front;
	}
	return CullingType::Double;
}

} // namespace Effekseer
