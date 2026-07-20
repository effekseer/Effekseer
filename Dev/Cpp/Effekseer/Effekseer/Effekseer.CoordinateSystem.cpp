#include "Effekseer.CoordinateSystem.h"
#include "Effekseer.RenderingTransform.h"
#include <cmath>

namespace Effekseer
{

namespace
{

Vector3D TransformVector(const Vector3D& value, const Matrix44& matrix)
{
	return Vector3D(
		value.X * matrix.Values[0][0] + value.Y * matrix.Values[1][0] + value.Z * matrix.Values[2][0],
		value.X * matrix.Values[0][1] + value.Y * matrix.Values[1][1] + value.Z * matrix.Values[2][1],
		value.X * matrix.Values[0][2] + value.Y * matrix.Values[1][2] + value.Z * matrix.Values[2][2]);
}

Matrix44 ToMatrix44(const Matrix43& value)
{
	Matrix44 result;
	for (int32_t row = 0; row < 4; row++)
	{
		for (int32_t column = 0; column < 3; column++)
		{
			result.Values[row][column] = value.Value[row][column];
		}
	}
	return result;
}

Matrix43 ToMatrix43(const Matrix44& value)
{
	Matrix43 result;
	for (int32_t row = 0; row < 4; row++)
	{
		for (int32_t column = 0; column < 3; column++)
		{
			result.Value[row][column] = value.Values[row][column];
		}
	}
	return result;
}

Matrix44 Multiply(const Matrix44& lhs, const Matrix44& rhs)
{
	Matrix44 result;
	Matrix44::Mul(result, lhs, rhs);
	return result;
}

Matrix43 ConvertTransform(const Matrix43& value, const Matrix44& fromBasis, const Matrix44& toBasis)
{
	return ToMatrix43(Multiply(Multiply(fromBasis, ToMatrix44(value)), toBasis));
}

Vector3D ConvertScale(const Vector3D& value, const Matrix44& axisTransform)
{
	const float source[3] = {value.X, value.Y, value.Z};
	float result[3] = {};
	for (int32_t destinationAxis = 0; destinationAxis < 3; destinationAxis++)
	{
		for (int32_t sourceAxis = 0; sourceAxis < 3; sourceAxis++)
		{
			result[destinationAxis] += std::abs(axisTransform.Values[destinationAxis][sourceAxis]) * source[sourceAxis];
		}
	}
	return Vector3D(result[0], result[1], result[2]);
}

} // namespace

bool IsValidCoordinateSystemMatrix(const Matrix44& matrix, float epsilon)
{
	if (!IsValidRenderingCoordinateMatrix(matrix, epsilon))
	{
		return false;
	}

	for (int32_t row = 0; row < 3; row++)
	{
		int32_t axisCount = 0;
		for (int32_t column = 0; column < 3; column++)
		{
			const auto magnitude = std::abs(matrix.Values[row][column]);
			if (magnitude <= epsilon)
			{
				continue;
			}
			if (std::abs(magnitude - 1.0f) > epsilon)
			{
				return false;
			}
			axisCount++;
		}
		if (axisCount != 1)
		{
			return false;
		}
	}

	for (int32_t column = 0; column < 3; column++)
	{
		int32_t axisCount = 0;
		for (int32_t row = 0; row < 3; row++)
		{
			if (std::abs(matrix.Values[row][column]) > epsilon)
			{
				axisCount++;
			}
		}
		if (axisCount != 1)
		{
			return false;
		}
	}

	return true;
}

CoordinateSystemConverter::CoordinateSystemConverter()
{
}

CoordinateSystemConverter CoordinateSystemConverter::FromCoordinateSystem(CoordinateSystem externalCoordinateSystem)
{
	Matrix44 matrix;
	if (externalCoordinateSystem == CoordinateSystem::LH)
	{
		matrix.Scaling(1.0f, 1.0f, -1.0f);
	}
	return FromMatrix(matrix);
}

CoordinateSystemConverter CoordinateSystemConverter::FromMatrix(const Matrix44& internalToExternal)
{
	CoordinateSystemConverter result;
	result.isValid_ = IsValidCoordinateSystemMatrix(internalToExternal);
	if (!result.isValid_)
	{
		return result;
	}

	result.internalToExternal_ = internalToExternal;
	result.externalToInternal_ = internalToExternal;
	result.externalToInternal_.Transpose();

	const auto& m = internalToExternal.Values;
	const float determinant =
		m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
		m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
		m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
	result.reversesWinding_ = determinant < 0.0f;
	return result;
}

bool CoordinateSystemConverter::IsValid() const
{
	return isValid_;
}

bool CoordinateSystemConverter::ReversesWinding() const
{
	return reversesWinding_;
}

const Matrix44& CoordinateSystemConverter::GetInternalToExternal() const
{
	return internalToExternal_;
}

const Matrix44& CoordinateSystemConverter::GetExternalToInternal() const
{
	return externalToInternal_;
}

Vector3D CoordinateSystemConverter::ToInternalPosition(const Vector3D& value) const
{
	return TransformVector(value, externalToInternal_);
}

Vector3D CoordinateSystemConverter::ToExternalPosition(const Vector3D& value) const
{
	return TransformVector(value, internalToExternal_);
}

Vector3D CoordinateSystemConverter::ToInternalDirection(const Vector3D& value) const
{
	return TransformVector(value, externalToInternal_);
}

Vector3D CoordinateSystemConverter::ToExternalDirection(const Vector3D& value) const
{
	return TransformVector(value, internalToExternal_);
}

Vector3D CoordinateSystemConverter::ToInternalPseudoVector(const Vector3D& value) const
{
	const auto converted = ToInternalDirection(value);
	return reversesWinding_ ? Vector3D(-converted.X, -converted.Y, -converted.Z) : converted;
}

Vector3D CoordinateSystemConverter::ToExternalPseudoVector(const Vector3D& value) const
{
	const auto converted = ToExternalDirection(value);
	return reversesWinding_ ? Vector3D(-converted.X, -converted.Y, -converted.Z) : converted;
}

Matrix43 CoordinateSystemConverter::ToInternalTransform(const Matrix43& value) const
{
	return ConvertTransform(value, internalToExternal_, externalToInternal_);
}

Matrix43 CoordinateSystemConverter::ToExternalTransform(const Matrix43& value) const
{
	return ConvertTransform(value, externalToInternal_, internalToExternal_);
}

Matrix43 CoordinateSystemConverter::ToInternalRotation(const Matrix43& value) const
{
	auto result = ToInternalTransform(value);
	result.Value[3][0] = 0.0f;
	result.Value[3][1] = 0.0f;
	result.Value[3][2] = 0.0f;
	return result;
}

Matrix43 CoordinateSystemConverter::ToExternalRotation(const Matrix43& value) const
{
	auto result = ToExternalTransform(value);
	result.Value[3][0] = 0.0f;
	result.Value[3][1] = 0.0f;
	result.Value[3][2] = 0.0f;
	return result;
}

Vector3D CoordinateSystemConverter::ToInternalScale(const Vector3D& value) const
{
	return ConvertScale(value, internalToExternal_);
}

Vector3D CoordinateSystemConverter::ToExternalScale(const Vector3D& value) const
{
	return ConvertScale(value, externalToInternal_);
}

Matrix44 CoordinateSystemConverter::ToInternalView(const Matrix44& value) const
{
	return Multiply(internalToExternal_, value);
}

Matrix44 CoordinateSystemConverter::ToExternalView(const Matrix44& value) const
{
	return Multiply(externalToInternal_, value);
}

} // namespace Effekseer
