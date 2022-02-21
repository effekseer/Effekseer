#include <Effekseer.h>

#include <Effekseer.SIMD.h>

#include "../TestHelper.h"

void TestQuaternionf()
{
	using namespace Effekseer::SIMD;

	const auto eps = 0.00001f;
	{
		const auto q = Quaternionf::FromMatrix(Mat44f::RotationX(Effekseer::EFK_PI / 2.0f));
		if (fabsf(q.GetX() - sin(Effekseer::EFK_PI / 4.0f)) > eps)
		{
			throw "Failed";
		}

		if (fabsf(q.GetY() - 0.0f) > eps)
		{
			throw "Failed";
		}

		if (fabsf(q.GetZ() - 0.0f) > eps)
		{
			throw "Failed";
		}
	}
}

TestRegister Math_TestQuaternionf("Math.Quaternionf", []() -> void { TestQuaternionf(); });
