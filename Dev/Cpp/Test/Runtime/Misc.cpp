#include "Effekseer/Geometry/GeometryUtility.h"

#include "../TestHelper.h"

void TestGeometryUtility()
{
	const auto getProjectionMatrix = [](float ovY, float aspect, float zn, float zf, bool isRightHand)
	{
		Effekseer::Matrix44 projMat;
		if (isRightHand)
		{
			projMat.PerspectiveFovRH(ovY / 180.0f * 3.14f, aspect, zn, zf);
		}
		else
		{
			projMat.PerspectiveFovLH(ovY / 180.0f * 3.14f, aspect, zn, zf);
		}

		return projMat;
	};

	Effekseer::Matrix44 cameraMat;
	cameraMat.LookAtRH({0, 0, 20}, {0, 0, 0}, {0, 1, 0});

	Effekseer::Matrix44 projMat;
	projMat.PerspectiveFovRH(90.0f / 180.0f * 3.14f, 4.0f / 3.0f, 1.0f, 30.0f);

	Effekseer::Sphare sphere;
	sphere.Center = {0, 0, 0};
	sphere.Radius = 5;

	Effekseer::Matrix44 cameraProjMat;
	Effekseer::Matrix44::Mul(cameraProjMat, cameraMat, projMat);
	const auto planes = Effekseer::GeometryUtility::CalculateFrustumPlanes(cameraProjMat, 0.0f, 1.0f);

	if (!Effekseer::GeometryUtility::IsContain(planes, sphere))
	{
		throw "Failed";
	}

	sphere.Center = {0, 0, -10};
	if (!Effekseer::GeometryUtility::IsContain(planes, sphere))
	{
		throw "Failed";
	}

	sphere.Center = {0, 0, -20};
	if (Effekseer::GeometryUtility::IsContain(planes, sphere))
	{
		throw "Failed";
	}

	sphere.Center = {0, 0, -30};
	if (Effekseer::GeometryUtility::IsContain(planes, sphere))
	{
		throw "Failed";
	}
}

TestRegister Misc_TestGeometryUtility("Misc.TestGeometryUtility", []() -> void
									  { TestGeometryUtility(); });
