#include <EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h>

#include "../TestHelper.h"

#include <cmath>

namespace EffekseerRenderer
{

} // namespace EffekseerRenderer

template <typename T>
void VertexTest(bool isValidValue)
{
	auto v = T();
	EffekseerRenderer::SetVertexAlphaUV(v, 1.0f, 0);
	EffekseerRenderer::SetVertexAlphaUV(v, 2.0f, 1);

	EffekseerRenderer::SetVertexUVDistortionUV(v, 3.0f, 0);
	EffekseerRenderer::SetVertexUVDistortionUV(v, 4.0f, 1);

	EffekseerRenderer::SetVertexBlendUV(v, 5.0f, 0);
	EffekseerRenderer::SetVertexBlendUV(v, 6.0f, 1);

	EffekseerRenderer::SetVertexBlendAlphaUV(v, 7.0f, 0);
	EffekseerRenderer::SetVertexBlendAlphaUV(v, 8.0f, 1);

	EffekseerRenderer::SetVertexBlendUVDistortionUV(v, 9.0f, 0);
	EffekseerRenderer::SetVertexBlendUVDistortionUV(v, 10.0f, 1);

	EffekseerRenderer::SetVertexFlipbookIndexAndNextRate(v, 11.0f);

	EffekseerRenderer::SetVertexAlphaThreshold(v, 12.0f);

	auto alphaUV = EffekseerRenderer::GetVertexAlphaUV(v);
	auto uvdistUV = EffekseerRenderer::GetVertexUVDistortionUV(v);

	auto blendUV = EffekseerRenderer::GetVertexBlendUV(v);
	auto blendAlphaUV = EffekseerRenderer::GetVertexBlendAlphaUV(v);
	auto blendUVDistUV = EffekseerRenderer::GetVertexBlendUVDistortionUV(v);

	auto flip = EffekseerRenderer::GetVertexFlipbookIndexAndNextRate(v);

	auto alphaThreshold = EffekseerRenderer::GetVertexAlphaThreshold(v);

	auto getExpect = [&](float value) -> float
	{
		if (isValidValue)
		{
			return value;
		}
		return 0.0f;
	};

	EXPECT_TRUE(alphaUV[0] == getExpect(1.0f) && alphaUV[1] == getExpect(2.0f))
	EXPECT_TRUE(uvdistUV[0] == getExpect(3.0f) && uvdistUV[1] == getExpect(4.0f))
	EXPECT_TRUE(blendUV[0] == getExpect(5.0f) && blendUV[1] == getExpect(6.0f))
	EXPECT_TRUE(blendAlphaUV[0] == getExpect(7.0f) && blendAlphaUV[1] == getExpect(8.0f))
	EXPECT_TRUE(blendUVDistUV[0] == getExpect(9.0f) && blendUVDistUV[1] == getExpect(10.0f))
	EXPECT_TRUE(flip == getExpect(11.0f))
	EXPECT_TRUE(alphaThreshold == getExpect(12.0f))
}

void VertexTest()
{
	VertexTest<EffekseerRenderer::AdvancedSimpleVertex>(true);
	VertexTest<EffekseerRenderer::AdvancedLightingVertex>(true);
	// VertexTest<EffekseerRenderer::AdvancedVertexDistortion>(true);
	VertexTest<EffekseerRenderer::SimpleVertex>(false);
	VertexTest<EffekseerRenderer::LightingVertex>(false);
	// VertexTest<EffekseerRenderer::VertexDistortion>(false);
	VertexTest<EffekseerRenderer::DynamicVertex>(false);
}

void DirectionalBillboardParallelDirectionTest()
{
	using namespace Effekseer::SIMD;

	Effekseer::SIMD::Mat43f dst;
	Effekseer::SIMD::Vec3f s;
	Effekseer::SIMD::Vec3f R;
	Effekseer::SIMD::Vec3f F;

	EffekseerRenderer::CalcBillboard(
		Effekseer::BillboardType::DirectionalBillboard,
		dst,
		s,
		R,
		F,
		Effekseer::SIMD::Mat43f::Identity,
		Effekseer::SIMD::Vec3f(0.0f, 1.0f, 0.0f),
		Effekseer::SIMD::Vec3f(0.0f, 1.0f, 0.0f));

	const auto isFinite = [](const Effekseer::SIMD::Vec3f& v) {
		return std::isfinite(v.GetX()) && std::isfinite(v.GetY()) && std::isfinite(v.GetZ());
	};

	EXPECT_TRUE(isFinite(R));
	EXPECT_TRUE(isFinite(F));
	EXPECT_TRUE(!R.IsZero());
	EXPECT_TRUE(!F.IsZero());
	EXPECT_EQUAL_NEAR(Effekseer::SIMD::Vec3f::Dot(R, Effekseer::SIMD::Vec3f(0.0f, 1.0f, 0.0f)), 0.0f, 0.0001f);
	EXPECT_EQUAL_NEAR(Effekseer::SIMD::Vec3f::Dot(F, Effekseer::SIMD::Vec3f(0.0f, 1.0f, 0.0f)), 0.0f, 0.0001f);
}

void PackedTangentHandednessTest()
{
	using namespace Effekseer::SIMD;

	const Vec3f transformedNormal(0.0f, 0.0f, 1.0f);
	const Vec3f transformedTangent(-1.0f, 0.0f, 0.0f);
	const auto regularTangent = EffekseerRenderer::PackTangent(transformedTangent, false);
	const auto reflectedTangent = EffekseerRenderer::PackTangent(transformedTangent, true);

	EXPECT_TRUE(regularTangent.A == 255);
	EXPECT_TRUE(reflectedTangent.A == 0);

	const float handedness = static_cast<float>(reflectedTangent.A) / 255.0f * 2.0f - 1.0f;
	const auto reconstructedBinormal = Vec3f::Cross(transformedNormal, transformedTangent) * handedness;
	const Vec3f expectedReflectedBinormal(0.0f, 1.0f, 0.0f);
	EXPECT_EQUAL_NEAR(reconstructedBinormal.GetX(), expectedReflectedBinormal.GetX(), 0.0001f);
	EXPECT_EQUAL_NEAR(reconstructedBinormal.GetY(), expectedReflectedBinormal.GetY(), 0.0001f);
	EXPECT_EQUAL_NEAR(reconstructedBinormal.GetZ(), expectedReflectedBinormal.GetZ(), 0.0001f);
}

void RenderingCoordinateCameraTransformTest()
{
	using namespace Effekseer::SIMD;

	Effekseer::Matrix44 coordinateMatrix;
	coordinateMatrix.RotationX(0.7f);
	const auto coordinateTransform = Effekseer::CalculateRenderingCoordinateTransform(coordinateMatrix);
	EXPECT_TRUE(!coordinateTransform.ReversesCameraFront);

	const Vec3f externalFront(0.0f, 0.0f, -1.0f);
	const auto internalFront = EffekseerRenderer::TransformCameraFrontToEffectSpace(externalFront, coordinateTransform);
	const auto roundTrippedFront = EffekseerRenderer::TransformDirection(internalFront, coordinateTransform.Transform);
	EXPECT_EQUAL_NEAR(roundTrippedFront.GetX(), externalFront.GetX(), 0.0001f);
	EXPECT_EQUAL_NEAR(roundTrippedFront.GetY(), externalFront.GetY(), 0.0001f);
	EXPECT_EQUAL_NEAR(roundTrippedFront.GetZ(), externalFront.GetZ(), 0.0001f);

	const Mat44f externalCamera = Mat44f::RotationY(-0.35f) * Mat44f::Translation(1.0f, 2.0f, 3.0f);
	const auto internalCamera = EffekseerRenderer::TransformCameraMatrixToEffectSpace(externalCamera, coordinateTransform);
	const Vec3f internalPoint(2.0f, -3.0f, 4.0f);
	const auto transformedBeforeCamera = Vec3f::Transform(
		Vec3f::Transform(internalPoint, coordinateTransform.Transform), externalCamera);
	const auto transformedWithInternalCamera = Vec3f::Transform(internalPoint, internalCamera);
	EXPECT_EQUAL_NEAR(transformedWithInternalCamera.GetX(), transformedBeforeCamera.GetX(), 0.0001f);
	EXPECT_EQUAL_NEAR(transformedWithInternalCamera.GetY(), transformedBeforeCamera.GetY(), 0.0001f);
	EXPECT_EQUAL_NEAR(transformedWithInternalCamera.GetZ(), transformedBeforeCamera.GetZ(), 0.0001f);

	// A draw-only reflection changes where the camera is in effect space, but
	// does not change the handedness convention used by the paired camera.
	Effekseer::Matrix44 reflectZ;
	reflectZ.Scaling(1.0f, 1.0f, -1.0f);
	const auto drawReflection = Effekseer::CalculateRenderingCoordinateTransform(reflectZ);
	EXPECT_TRUE(drawReflection.ReversesWinding);
	EXPECT_TRUE(!drawReflection.ReversesCameraFront);
	Effekseer::Matrix44 rightHandedCamera;
	rightHandedCamera.LookAtRH({0.0f, 0.0f, 10.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
	const Vec3f rightHandedCameraFront(
		rightHandedCamera.Values[0][2],
		rightHandedCamera.Values[1][2],
		rightHandedCamera.Values[2][2]);
	const auto reflectedDrawFront = EffekseerRenderer::TransformCameraFrontToEffectSpace(
		rightHandedCameraFront, drawReflection);
	EXPECT_EQUAL_NEAR(reflectedDrawFront.GetZ(), -1.0f, 0.0001f);

	// The same reflection at an external coordinate-system boundary is paired
	// with an LH camera. Its view-matrix Z column has the opposite meaning, so
	// the convention correction restores Effekseer's canonical camera front.
	auto externalBoundary = drawReflection;
	externalBoundary.ReversesCulling = false;
	externalBoundary.ReversesCameraFront = externalBoundary.ReversesWinding;
	Effekseer::Matrix44 leftHandedCamera;
	leftHandedCamera.LookAtLH({0.0f, 0.0f, -10.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
	const Vec3f leftHandedCameraFront(
		leftHandedCamera.Values[0][2],
		leftHandedCamera.Values[1][2],
		leftHandedCamera.Values[2][2]);
	const auto boundaryFront = EffekseerRenderer::TransformCameraFrontToEffectSpace(
		leftHandedCameraFront, externalBoundary);
	EXPECT_EQUAL_NEAR(boundaryFront.GetX(), 0.0f, 0.0001f);
	EXPECT_EQUAL_NEAR(boundaryFront.GetY(), 0.0f, 0.0001f);
	EXPECT_EQUAL_NEAR(boundaryFront.GetZ(), 1.0f, 0.0001f);

	const Vec3f canonicalPosition(2.0f, 0.0f, 3.0f);
	const auto externalPosition = EffekseerRenderer::TransformDirection(
		canonicalPosition, externalBoundary.Transform);
	const auto normalizedExternalFront = EffekseerRenderer::NormalizeCameraFrontForRenderingSpace(
		leftHandedCameraFront, true, externalBoundary);
	EXPECT_EQUAL_NEAR(
		Vec3f::Dot(externalPosition, normalizedExternalFront),
		Vec3f::Dot(canonicalPosition, rightHandedCameraFront),
		0.0001f);
	EXPECT_TRUE(EffekseerRenderer::IsRenderingCameraRightHanded(true, drawReflection));
	EXPECT_TRUE(!EffekseerRenderer::IsRenderingCameraRightHanded(true, externalBoundary));
}

TestRegister Runtime_VertexTest("Runtime.Vertex", []() -> void
								{ VertexTest(); });
TestRegister Runtime_DirectionalBillboardParallelDirectionTest("Runtime.DirectionalBillboardParallelDirection", []() -> void
																  { DirectionalBillboardParallelDirectionTest(); });
TestRegister Runtime_PackedTangentHandednessTest("Runtime.PackedTangentHandedness", []() -> void
													 { PackedTangentHandednessTest(); });
TestRegister Runtime_RenderingCoordinateCameraTransformTest("Runtime.RenderingCoordinateCameraTransform", []() -> void
														 { RenderingCoordinateCameraTransformTest(); });
