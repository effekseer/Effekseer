#ifndef __EFFEKSEER_COORDINATE_SYSTEM_H__
#define __EFFEKSEER_COORDINATE_SYSTEM_H__

#include "Effekseer.Matrix43.h"
#include "Effekseer.Matrix44.h"
#include "Effekseer.Vector3D.h"

namespace Effekseer
{

enum class CoordinateSystemMode : int32_t
{
	LegacySimulation,
	ExternalConversion,
};

/**
	@brief Defines the conversion from Effekseer's internal RH space to an external coordinate space.
*/
struct CoordinateSystemTransform
{
	Matrix44 ToExternal;
};

/**
	@brief Converts values between Effekseer's internal RH space and an external coordinate space.

	The coordinate matrix is restricted to a signed axis permutation. Input and output transforms are
	derived from the same matrix so they cannot become inconsistent.
*/
class CoordinateSystemConverter
{
private:
	Matrix44 internalToExternal_;
	Matrix44 externalToInternal_;
	bool isValid_ = true;
	bool reversesWinding_ = false;

public:
	CoordinateSystemConverter();

	static CoordinateSystemConverter FromCoordinateSystem(CoordinateSystem externalCoordinateSystem);

	static CoordinateSystemConverter FromMatrix(const Matrix44& internalToExternal);

	bool IsValid() const;

	bool ReversesWinding() const;

	const Matrix44& GetInternalToExternal() const;

	const Matrix44& GetExternalToInternal() const;

	Vector3D ToInternalPosition(const Vector3D& value) const;

	Vector3D ToExternalPosition(const Vector3D& value) const;

	Vector3D ToInternalDirection(const Vector3D& value) const;

	Vector3D ToExternalDirection(const Vector3D& value) const;

	Vector3D ToInternalPseudoVector(const Vector3D& value) const;

	Vector3D ToExternalPseudoVector(const Vector3D& value) const;

	Matrix43 ToInternalTransform(const Matrix43& value) const;

	Matrix43 ToExternalTransform(const Matrix43& value) const;

	Matrix43 ToInternalRotation(const Matrix43& value) const;

	Matrix43 ToExternalRotation(const Matrix43& value) const;

	Vector3D ToInternalScale(const Vector3D& value) const;

	Vector3D ToExternalScale(const Vector3D& value) const;

	Matrix44 ToInternalView(const Matrix44& value) const;

	Matrix44 ToExternalView(const Matrix44& value) const;
};

bool IsValidCoordinateSystemMatrix(const Matrix44& matrix, float epsilon = 0.0001f);

} // namespace Effekseer

#endif // __EFFEKSEER_COORDINATE_SYSTEM_H__
