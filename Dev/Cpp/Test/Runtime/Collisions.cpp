#include "Effekseer/Parameter/Effekseer.Collisions.h"
#include "Effekseer.h"

#include "../TestHelper.h"

namespace
{

void TestExternalCollisionUsesSurfaceNormal()
{
	Effekseer::CollisionsState state;
	Effekseer::CollisionsParameter parameter;
	parameter.IsSceneCollisionWithExternal = true;
	parameter.Bounce = {1.0f, 1.0f};
	parameter.Friction = {0.0f, 0.0f};
	bool callbackCalled = false;

	const auto result = Effekseer::CollisionsFunctions::Update(
		state,
		parameter,
		Effekseer::SIMD::Vec3f(-1.0f, 0.0f, 0.0f),
		Effekseer::SIMD::Vec3f(1.0f, 0.0f, 0.0f),
		Effekseer::SIMD::Vec3f(-2.0f, 1.0f, 0.0f),
		Effekseer::SIMD::Vec3f(0.0f, 0.0f, 0.0f),
		1.0f,
		[&callbackCalled](const Effekseer::Vector3D& start, const Effekseer::Vector3D& end, Effekseer::Vector3D& collisionPosition, Effekseer::Vector3D& collisionNormal) -> bool
		{
			callbackCalled = true;

			collisionPosition = Effekseer::Vector3D(0.0f, 0.0f, 0.0f);
			collisionNormal = Effekseer::Vector3D(2.0f, 0.0f, 0.0f);
			return true;
		});

	EXPECT_TRUE(callbackCalled);
	EXPECT_TRUE(state.CollidedThisFrame);
	EXPECT_EQUAL_NEAR(result.PositionChange.GetX(), -1.0f, 0.0001f);
	EXPECT_EQUAL_NEAR(result.PositionChange.GetY(), 0.0f, 0.0001f);
	EXPECT_EQUAL_NEAR(result.PositionChange.GetZ(), 0.0f, 0.0001f);
	EXPECT_EQUAL_NEAR(result.VelocityChange.GetX(), 4.0f, 0.01f);
	EXPECT_EQUAL_NEAR(result.VelocityChange.GetY(), 0.0f, 0.0001f);
	EXPECT_EQUAL_NEAR(result.VelocityChange.GetZ(), 0.0f, 0.0001f);
}

TestRegister Collisions_TestExternalCollisionUsesSurfaceNormal("Collisions.TestExternalCollisionUsesSurfaceNormal", []() -> void
														 { TestExternalCollisionUsesSurfaceNormal(); });

} // namespace
