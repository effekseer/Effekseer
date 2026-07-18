#include "Effekseer/Geometry/Effekseer.GeometryUtility.h"
#include "Effekseer/Parameter/Effekseer.Collisions.h"
#include "Effekseer.h"

#include "../TestHelper.h"

#include <array>

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

	const auto isRH = [](CoordinateStyleType style)
	{
		if (style == CoordinateStyleType::RH || style == CoordinateStyleType::RH_Reversed)
		{
			return true;
		}
		return false;
	};

	const auto isRevresed = [](CoordinateStyleType style)
	{
		if (style == CoordinateStyleType::RH_Reversed)
		{
			return true;
		}
		return false;
	};

	const auto getSystem = [](CoordinateStyleType style)
	{
		if (style == CoordinateStyleType::RH || style == CoordinateStyleType::RH_Reversed)
		{
			return Effekseer::CoordinateSystem::RH;
		}
		return Effekseer::CoordinateSystem::LH;
	};

	const auto getReversedPerspectiveMatrixRH = [](float ovY, float aspect, float zn, float zf)
	{
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

	const auto getProjectionMatrix = [&](float ovY, float aspect, float zn, float zf, CoordinateStyleType coodinateStyle)
	{
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

	const auto getCameraMatrix = [&](Effekseer::Vector3D position, Effekseer::Vector3D focus, Effekseer::Vector3D up, CoordinateStyleType coodinateStyle)
	{
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

	const auto getCoordinatePosition = [](Effekseer::Vector3D position, bool isRightHand)
	{
		if (!isRightHand)
		{
			position.Z = -position.Z;
		}

		return position;
	};

	const auto run = [&](State state)
	{
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

void TestEffectFlip()
{
	{
		auto manager = Effekseer::Manager::Create(1);
		Effekseer::Manager::PlayParameter playParameter;
		playParameter.Flip = {true, false, true};
		const Effekseer::EffectFlipParameter expectedFlip{true, false, true};
		EXPECT_TRUE(playParameter.Flip == expectedFlip);
		manager->SetEffectFlip(-1, playParameter.Flip);
		const Effekseer::EffectFlipParameter defaultFlip{};
		EXPECT_TRUE(manager->GetEffectFlip(-1) == defaultFlip);
	}
	{
		struct EffectFlipTestCase
		{
			const char16_t* Path;
			Effekseer::EffectFlipParameter InitialFlip;
			Effekseer::EffectFlipParameter ChangedFlip;
		};

		const std::array<EffectFlipTestCase, 6> testCases = {{
			{u"TestData/Effects/Update_17x/Sprite.efkefc", {true, false, false}, {false, true, true}},
			{u"ResourceData/samples/00_Basic/Simple_Ribbon_Sword.efkefc", {false, true, false}, {true, false, true}},
			{u"ResourceData/samples/00_Basic/Simple_Ring_Shape1.efkefc", {false, false, true}, {true, true, false}},
			{u"TestData/Effects/Update_17x/Track.efkefc", {true, true, false}, {false, false, true}},
			{u"TestData/Effects/Update_17x/Model.efkefc", {true, false, true}, {false, true, false}},
			{u"TestData/Effects/18/GpuParticles_sprite_simple.efkefc", {false, true, true}, {true, false, false}},
		}};

		auto manager = Effekseer::Manager::Create(256);
		const auto rootPath = GetDirectoryPathAsU16(__FILE__) + u"../../../../";
		std::array<Effekseer::EffectRef, testCases.size()> effects;
		std::array<Effekseer::Handle, testCases.size()> handles;
		std::array<Effekseer::Matrix43, testCases.size()> simulationMatrices;

		for (size_t i = 0; i < testCases.size(); i++)
		{
			const auto path = rootPath + testCases[i].Path;
			effects[i] = Effekseer::Effect::Create(manager, path.c_str());
			EXPECT_TRUE(effects[i] != nullptr);

			Effekseer::Manager::PlayParameter playParameter;
			playParameter.Effect = effects[i];
			playParameter.Position = {
				static_cast<float>(i * 10 + 1),
				static_cast<float>(i * 10 + 2),
				static_cast<float>(i * 10 + 3)};
			playParameter.Flip = testCases[i].InitialFlip;
			handles[i] = manager->Play(playParameter);
			EXPECT_TRUE(handles[i] >= 0);
			EXPECT_TRUE(manager->GetEffectFlip(handles[i]) == testCases[i].InitialFlip);
			simulationMatrices[i] = manager->GetMatrix(handles[i]);
		}

		for (size_t changedIndex = 0; changedIndex < testCases.size(); changedIndex++)
		{
			manager->SetEffectFlip(handles[changedIndex], testCases[changedIndex].ChangedFlip);

			for (size_t i = 0; i < testCases.size(); i++)
			{
				const auto expectedFlip = i <= changedIndex ? testCases[i].ChangedFlip : testCases[i].InitialFlip;
				EXPECT_TRUE(manager->GetEffectFlip(handles[i]) == expectedFlip);

				const auto unchangedSimulationMatrix = manager->GetMatrix(handles[i]);
				EXPECT_TRUE(memcmp(&simulationMatrices[i], &unchangedSimulationMatrix, sizeof(unchangedSimulationMatrix)) == 0);
			}
		}

		manager->StopAllEffects();
	}

	const auto rootMatrix = Effekseer::SIMD::Mat43f::SRT(
		Effekseer::SIMD::Vec3f(2.0f, 3.0f, 4.0f),
		Effekseer::SIMD::Mat43f::RotationZXY(0.4f, -0.2f, 0.7f),
		Effekseer::SIMD::Vec3f(10.0f, 20.0f, 30.0f));
	const Effekseer::SIMD::Vec3f localPoint(1.0f, 2.0f, 3.0f);
	const auto worldPoint = Effekseer::SIMD::Vec3f::Transform(localPoint, rootMatrix);

	for (int32_t bits = 0; bits < 8; bits++)
	{
		Effekseer::EffectFlipParameter flip;
		flip.FlipX = (bits & 1) != 0;
		flip.FlipY = (bits & 2) != 0;
		flip.FlipZ = (bits & 4) != 0;

		const auto renderingTransform = Effekseer::CalculateEffectRenderingTransform(rootMatrix, flip);
		EXPECT_TRUE(renderingTransform.IsEnabled == (bits != 0));
		const auto transformed = Effekseer::SIMD::Vec3f::Transform(worldPoint, renderingTransform.Transform);
		const auto expected = Effekseer::SIMD::Vec3f::Transform(
			Effekseer::SIMD::Vec3f(
				flip.FlipX ? -localPoint.GetX() : localPoint.GetX(),
				flip.FlipY ? -localPoint.GetY() : localPoint.GetY(),
				flip.FlipZ ? -localPoint.GetZ() : localPoint.GetZ()),
			rootMatrix);
		EXPECT_EQUAL_NEAR(transformed.GetX(), expected.GetX(), 0.0001f);
		EXPECT_EQUAL_NEAR(transformed.GetY(), expected.GetY(), 0.0001f);
		EXPECT_EQUAL_NEAR(transformed.GetZ(), expected.GetZ(), 0.0001f);
		EXPECT_TRUE(renderingTransform.ReversesWinding == (flip.FlipX ^ flip.FlipY ^ flip.FlipZ));
	}

	EXPECT_TRUE(Effekseer::SIMD::Mat43f::Equal(
		rootMatrix,
		Effekseer::SIMD::Mat43f::SRT(
			Effekseer::SIMD::Vec3f(2.0f, 3.0f, 4.0f),
			Effekseer::SIMD::Mat43f::RotationZXY(0.4f, -0.2f, 0.7f),
			Effekseer::SIMD::Vec3f(10.0f, 20.0f, 30.0f))));
}

TestRegister Misc_TestGeometryUtility("Misc.TestGeometryUtility", []() -> void
									  { TestGeometryUtility(); });

TestRegister Misc_TestExternalCollisionUsesSurfaceNormal("Misc.TestExternalCollisionUsesSurfaceNormal", []() -> void
													 { TestExternalCollisionUsesSurfaceNormal(); });

TestRegister Misc_TestEffectFlip("Misc.TestEffectFlip", []() -> void
								   { TestEffectFlip(); });
