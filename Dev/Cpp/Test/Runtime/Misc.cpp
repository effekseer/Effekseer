#include "Effekseer/Geometry/GeometryUtility.h"

#include "../TestHelper.h"

void TestGeometryUtility()
{
	enum class CoordinateStyleType
	{
		RH,
		LH,
		RH_Reversed,
	};

	struct State
	{
		CoordinateStyleType coordinateStyle;
		bool isReversed = false;
		Effekseer::Matrix44 transform;
	};

	const auto isRH = [](CoordinateStyleType style) {
		if (style == CoordinateStyleType::RH || style == CoordinateStyleType::RH_Reversed)
		{
			return true;
		}
		return false;
	};

	const auto isRevresed = [](CoordinateStyleType style) {
		if (style == CoordinateStyleType::RH_Reversed)
		{
			return true;
		}
		return false;
	};

	const auto getSystem = [](CoordinateStyleType style) {
		if (style == CoordinateStyleType::RH || style == CoordinateStyleType::RH_Reversed)
		{
			return Effekseer::CoordinateSystem::RH;
		}
		return Effekseer::CoordinateSystem::LH;
	};

	const auto getReversedPerspectiveMatrixRH = [](float ovY, float aspect, float zn, float zf) {
		Effekseer::Matrix44 mat;
		float yScale = 1 / tanf(ovY / 2);
		float xScale = yScale / aspect;

		mat.Values[0][0] = xScale;
		mat.Values[0][1] = 0;
		mat.Values[0][2] = 0;
		mat.Values[0][3] = 0;

		mat.Values[1][0] = 0;
		mat.Values[1][1] = yScale;
		mat.Values[1][2] = 0;
		mat.Values[1][3] = 0;

		mat.Values[2][0] = 0;
		mat.Values[2][1] = 0;
		mat.Values[2][2] = zn / (zf - zn);
		mat.Values[2][3] = -1;

		mat.Values[3][0] = 0;
		mat.Values[3][1] = 0;
		mat.Values[3][2] = zn * zf / (zf - zn);
		mat.Values[3][3] = 0;
		return mat;
	};

	const auto getProjectionMatrix = [&](float ovY, float aspect, float zn, float zf, CoordinateStyleType coodinateStyle) {
		Effekseer::Matrix44 projMat;
		if (coodinateStyle == CoordinateStyleType::RH)
		{
			projMat.PerspectiveFovRH(ovY / 180.0f * 3.14f, aspect, zn, zf);
		}
		else if (coodinateStyle == CoordinateStyleType::RH_Reversed)
		{
			projMat = getReversedPerspectiveMatrixRH(ovY / 180.0f * 3.14f, aspect, zn, zf);
		}
		else
		{
			projMat.PerspectiveFovLH(ovY / 180.0f * 3.14f, aspect, zn, zf);
		}

		return projMat;
	};

	const auto getCameraMatrix = [&](Effekseer::Vector3D position, Effekseer::Vector3D focus, Effekseer::Vector3D up, CoordinateStyleType coodinateStyle) {
		Effekseer::Matrix44 cameraMat;
		if (isRH(coodinateStyle))
		{
			cameraMat.LookAtRH(position, focus, up);
		}
		else
		{
			cameraMat.LookAtLH(position, focus, up);
		}

		return cameraMat;
	};

	const auto getCoordinatePosition = [](Effekseer::Vector3D position, bool isRightHand) {
		if (!isRightHand)
		{
			position.Z = -position.Z;
		}

		return position;
	};

	const auto run = [&](State state) {
		bool isRightHand = isRH(state.coordinateStyle);

		Effekseer::Vector3D cameraPos = getCoordinatePosition({0, 0, 20}, isRightHand);
		Effekseer::Vector3D cameraFocus = getCoordinatePosition({0, 0, 0}, isRightHand);
		Effekseer::Vector3D cameraUp = {0, 1, 0};
		Effekseer::Vector3D zero = {0, 0, 0};

		Effekseer::Vector3D::Transform(cameraPos, cameraPos, state.transform);
		Effekseer::Vector3D::Transform(cameraFocus, cameraFocus, state.transform);
		Effekseer::Vector3D::Transform(cameraUp, cameraUp, state.transform);
		Effekseer::Vector3D::Transform(zero, zero, state.transform);

		Effekseer::Matrix44 cameraMat = getCameraMatrix(cameraPos, cameraFocus, cameraUp - zero, state.coordinateStyle);
		Effekseer::Matrix44 projMat = getProjectionMatrix(90.0f, 4.0f / 3.0, 1.0f, 30.0f, state.coordinateStyle);

		Effekseer::Sphare sphere;
		sphere.Center = {0, 0, 0};
		sphere.Radius = 5;

		Effekseer::Matrix44 cameraProjMat;
		Effekseer::Matrix44::Mul(cameraProjMat, cameraMat, projMat);

		float znear = 0.0f;
		float zfar = 1.0f;

		if (isRevresed(state.coordinateStyle))
		{
			std::swap(znear, zfar);
		}

		const auto planes = Effekseer::GeometryUtility::CalculateFrustumPlanes(cameraProjMat, znear, zfar, getSystem(state.coordinateStyle));

		if (!Effekseer::GeometryUtility::IsContain(planes, sphere))
		{
			throw "Failed";
		}

		Effekseer::Vector3D::Transform(sphere.Center, getCoordinatePosition({0, 0, -10}, isRightHand), state.transform);
		if (!Effekseer::GeometryUtility::IsContain(planes, sphere))
		{
			throw "Failed";
		}

		Effekseer::Vector3D::Transform(sphere.Center, getCoordinatePosition({0, 0, -20}, isRightHand), state.transform);
		if (Effekseer::GeometryUtility::IsContain(planes, sphere))
		{
			throw "Failed";
		}

		Effekseer::Vector3D::Transform(sphere.Center, getCoordinatePosition({0, 0, 30}, isRightHand), state.transform);
		if (Effekseer::GeometryUtility::IsContain(planes, sphere))
		{
			throw "Failed";
		}

		Effekseer::Vector3D::Transform(sphere.Center, getCoordinatePosition({0, 25, 0}, isRightHand), state.transform);
		if (!Effekseer::GeometryUtility::IsContain(planes, sphere))
		{
			throw "Failed";
		}

		Effekseer::Vector3D::Transform(sphere.Center, getCoordinatePosition({0, 28, 0}, isRightHand), state.transform);
		if (Effekseer::GeometryUtility::IsContain(planes, sphere))
		{
			throw "Failed";
		}

		Effekseer::Vector3D::Transform(sphere.Center, getCoordinatePosition({0, -25, 0}, isRightHand), state.transform);
		if (!Effekseer::GeometryUtility::IsContain(planes, sphere))
		{
			throw "Failed";
		}

		Effekseer::Vector3D::Transform(sphere.Center, getCoordinatePosition({0, -28, 0}, isRightHand), state.transform);
		if (Effekseer::GeometryUtility::IsContain(planes, sphere))
		{
			throw "Failed";
		}

		Effekseer::Vector3D::Transform(sphere.Center, getCoordinatePosition({34, 0, 0}, isRightHand), state.transform);
		if (!Effekseer::GeometryUtility::IsContain(planes, sphere))
		{
			throw "Failed";
		}

		Effekseer::Vector3D::Transform(sphere.Center, getCoordinatePosition({38, 0, 0}, isRightHand), state.transform);
		if (Effekseer::GeometryUtility::IsContain(planes, sphere))
		{
			throw "Failed";
		}

		Effekseer::Vector3D::Transform(sphere.Center, getCoordinatePosition({-34, 0, 0}, isRightHand), state.transform);
		if (!Effekseer::GeometryUtility::IsContain(planes, sphere))
		{
			throw "Failed";
		}

		Effekseer::Vector3D::Transform(sphere.Center, getCoordinatePosition({-38, 0, 0}, isRightHand), state.transform);
		if (Effekseer::GeometryUtility::IsContain(planes, sphere))
		{
			throw "Failed";
		}
	};

	for (size_t i = 0; i < 8; i++)
	{
		State state;
		state.coordinateStyle = CoordinateStyleType::RH;
		state.transform.RotationX(i * 3.14f / 4.0f);
		run(state);

		state.coordinateStyle = CoordinateStyleType::LH;
		run(state);

		state.coordinateStyle = CoordinateStyleType::RH_Reversed;
		run(state);
	}

	for (size_t i = 0; i < 8; i++)
	{
		State state;
		state.coordinateStyle = CoordinateStyleType::RH;
		state.transform.RotationY(i * 3.14f / 4.0f);
		run(state);

		state.coordinateStyle = CoordinateStyleType::LH;
		run(state);

		state.coordinateStyle = CoordinateStyleType::RH_Reversed;
		run(state);
	}

	for (size_t i = 0; i < 8; i++)
	{
		State state;
		state.coordinateStyle = CoordinateStyleType::RH;
		state.transform.RotationZ(i * 3.14f / 4.0f);
		run(state);

		state.coordinateStyle = CoordinateStyleType::LH;
		run(state);

		state.coordinateStyle = CoordinateStyleType::RH_Reversed;
		run(state);
	}
}

TestRegister Misc_TestGeometryUtility("Misc.TestGeometryUtility", []() -> void { TestGeometryUtility(); });
