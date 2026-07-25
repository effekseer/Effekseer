#include "Effekseer.h"
#include "Effekseer/Effekseer.Instance.h"

#include "../TestHelper.h"

#include <array>
#include <vector>

namespace
{

void ExpectOrthogonalLinearPart(const Effekseer::SIMD::Mat43f& matrix)
{
	const auto value = Effekseer::SIMD::ToStruct(matrix);
	for (int32_t row = 0; row < 3; row++)
	{
		for (int32_t other = row; other < 3; other++)
		{
			float dot = 0.0f;
			for (int32_t column = 0; column < 3; column++)
			{
				dot += value.Value[row][column] * value.Value[other][column];
			}
			EXPECT_EQUAL_NEAR(dot, row == other ? 1.0f : 0.0f, 0.0001f);
		}
	}
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
		EXPECT_TRUE(!renderingTransform.ReversesCameraFront);
		EXPECT_TRUE(renderingTransform.ReversesCulling == renderingTransform.ReversesWinding);
	}

	EXPECT_TRUE(Effekseer::SIMD::Mat43f::Equal(
		rootMatrix,
		Effekseer::SIMD::Mat43f::SRT(
			Effekseer::SIMD::Vec3f(2.0f, 3.0f, 4.0f),
			Effekseer::SIMD::Mat43f::RotationZXY(0.4f, -0.2f, 0.7f),
			Effekseer::SIMD::Vec3f(10.0f, 20.0f, 30.0f))));

	// GlobalMatrix and BaseMatrix can each be a proper SRT while their product
	// contains shear. The resulting emitter flip must still be a rigid reflection.
	const auto globalMatrix = Effekseer::SIMD::Mat43f::SRT(
		Effekseer::SIMD::Vec3f(2.0f, 1.0f, 1.0f),
		Effekseer::SIMD::Mat43f::RotationZ(0.7f),
		Effekseer::SIMD::Vec3f(5.0f, 6.0f, 7.0f));
	const auto baseMatrix = Effekseer::SIMD::Mat43f::SRT(
		Effekseer::SIMD::Vec3f(1.0f, 3.0f, 1.0f),
		Effekseer::SIMD::Mat43f::RotationX(0.4f),
		Effekseer::SIMD::Vec3f(-2.0f, 1.0f, 4.0f));
	const auto shearedRoot = globalMatrix * baseMatrix;
	EXPECT_TRUE(!Effekseer::SIMD::ToStruct(shearedRoot).IsProperSRT());
	const auto shearedRootFlip = Effekseer::CalculateEffectRenderingTransform(shearedRoot, {true, false, false});
	ExpectOrthogonalLinearPart(shearedRootFlip.Transform);
	EXPECT_TRUE(shearedRootFlip.ReversesWinding);
	EXPECT_TRUE(!shearedRootFlip.ReversesCameraFront);
	EXPECT_TRUE(shearedRootFlip.ReversesCulling);

	const auto rootOrigin = Effekseer::SIMD::Vec3f::Transform(Effekseer::SIMD::Vec3f(0.0f), shearedRoot);
	const auto fixedOrigin = Effekseer::SIMD::Vec3f::Transform(rootOrigin, shearedRootFlip.Transform);
	EXPECT_EQUAL_NEAR(fixedOrigin.GetX(), rootOrigin.GetX(), 0.0001f);
	EXPECT_EQUAL_NEAR(fixedOrigin.GetY(), rootOrigin.GetY(), 0.0001f);
	EXPECT_EQUAL_NEAR(fixedOrigin.GetZ(), rootOrigin.GetZ(), 0.0001f);

	const Effekseer::SIMD::Vec3f arbitraryPoint(3.0f, -4.0f, 8.0f);
	const auto reflectedPoint = Effekseer::SIMD::Vec3f::Transform(arbitraryPoint, shearedRootFlip.Transform);
	const auto reflectedTwice = Effekseer::SIMD::Vec3f::Transform(reflectedPoint, shearedRootFlip.Transform);
	EXPECT_EQUAL_NEAR(reflectedTwice.GetX(), arbitraryPoint.GetX(), 0.0001f);
	EXPECT_EQUAL_NEAR(reflectedTwice.GetY(), arbitraryPoint.GetY(), 0.0001f);
	EXPECT_EQUAL_NEAR(reflectedTwice.GetZ(), arbitraryPoint.GetZ(), 0.0001f);
}

void TestRenderingCoordinateTransform()
{
	Effekseer::Matrix43 properSRT;
	properSRT.Indentity();
	EXPECT_TRUE(properSRT.IsProperSRT());

	Effekseer::Matrix43 improperSRT;
	improperSRT.Scaling(-1.0f, 1.0f, 1.0f);
	EXPECT_TRUE(!improperSRT.IsProperSRT());
	improperSRT.Scaling(0.0f, 1.0f, 1.0f);
	EXPECT_TRUE(!improperSRT.IsProperSRT());
	improperSRT.Indentity();
	improperSRT.Value[0][1] = 0.25f;
	EXPECT_TRUE(!improperSRT.IsProperSRT());

	Effekseer::Matrix44 identity;
	EXPECT_TRUE(Effekseer::IsValidRenderingCoordinateMatrix(identity));
	const auto identityTransform = Effekseer::CalculateRenderingCoordinateTransform(identity);
	EXPECT_TRUE(!identityTransform.IsEnabled);
	EXPECT_TRUE(!identityTransform.ReversesWinding);
	EXPECT_TRUE(!identityTransform.ReversesCameraFront);
	EXPECT_TRUE(!identityTransform.ReversesCulling);

	Effekseer::Matrix44 reflectY;
	reflectY.Scaling(1.0f, -1.0f, 1.0f);
	EXPECT_TRUE(Effekseer::IsValidRenderingCoordinateMatrix(reflectY));
	const auto reflectYTransform = Effekseer::CalculateRenderingCoordinateTransform(reflectY);
	EXPECT_TRUE(reflectYTransform.IsEnabled);
	EXPECT_TRUE(reflectYTransform.ReversesWinding);
	EXPECT_TRUE(!reflectYTransform.ReversesCameraFront);
	EXPECT_TRUE(reflectYTransform.ReversesCulling);
	EXPECT_TRUE(Effekseer::GetTransformedCullingType(Effekseer::CullingType::Front, reflectYTransform) == Effekseer::CullingType::Back);
	EXPECT_TRUE(Effekseer::GetTransformedCullingType(Effekseer::CullingType::Back, reflectYTransform) == Effekseer::CullingType::Front);
	EXPECT_TRUE(Effekseer::GetTransformedCullingType(Effekseer::CullingType::Double, reflectYTransform) == Effekseer::CullingType::Double);
	auto coordinateBoundaryTransform = reflectYTransform;
	coordinateBoundaryTransform.ReversesCulling = false;
	coordinateBoundaryTransform.ReversesCameraFront = coordinateBoundaryTransform.ReversesWinding;
	EXPECT_TRUE(Effekseer::GetTransformedCullingType(Effekseer::CullingType::Front, coordinateBoundaryTransform) == Effekseer::CullingType::Front);
	EXPECT_TRUE(Effekseer::GetTransformedCullingType(Effekseer::CullingType::Back, coordinateBoundaryTransform) == Effekseer::CullingType::Back);
	EXPECT_TRUE(Effekseer::GetTransformedCullingType(Effekseer::CullingType::Double, coordinateBoundaryTransform) == Effekseer::CullingType::Double);

	Effekseer::Matrix44 exchangeYZ;
	exchangeYZ.Values[1][1] = 0.0f;
	exchangeYZ.Values[1][2] = 1.0f;
	exchangeYZ.Values[2][1] = 1.0f;
	exchangeYZ.Values[2][2] = 0.0f;
	EXPECT_TRUE(Effekseer::IsValidRenderingCoordinateMatrix(exchangeYZ));
	const auto exchangeYZTransform = Effekseer::CalculateRenderingCoordinateTransform(exchangeYZ);
	EXPECT_TRUE(exchangeYZTransform.ReversesWinding);
	EXPECT_TRUE(!exchangeYZTransform.ReversesCameraFront);
	EXPECT_TRUE(exchangeYZTransform.ReversesCulling);

	Effekseer::Matrix44 translated;
	translated.Translation(1.0f, 2.0f, 3.0f);
	EXPECT_TRUE(!Effekseer::IsValidRenderingCoordinateMatrix(translated));

	Effekseer::Matrix44 scaled;
	scaled.Scaling(1.0f, 2.0f, 1.0f);
	EXPECT_TRUE(!Effekseer::IsValidRenderingCoordinateMatrix(scaled));

	Effekseer::Matrix44 sheared;
	sheared.Values[0][1] = 0.25f;
	EXPECT_TRUE(!Effekseer::IsValidRenderingCoordinateMatrix(sheared));

	const auto rootMatrix = Effekseer::SIMD::Mat43f::SRT(
		Effekseer::SIMD::Vec3f(2.0f, 3.0f, 4.0f),
		Effekseer::SIMD::Mat43f::RotationZXY(0.4f, -0.2f, 0.7f),
		Effekseer::SIMD::Vec3f(10.0f, 20.0f, 30.0f));
	const auto effectTransform = Effekseer::CalculateEffectRenderingTransform(rootMatrix, {true, false, false});
	const auto composedTransform = Effekseer::ComposeRenderingTransforms(effectTransform, reflectYTransform);
	EXPECT_TRUE(composedTransform.IsEnabled);
	EXPECT_TRUE(!composedTransform.ReversesWinding);
	EXPECT_TRUE(!composedTransform.ReversesCameraFront);
	EXPECT_TRUE(!composedTransform.ReversesCulling);
	const auto composedBoundaryTransform = Effekseer::ComposeRenderingTransforms(effectTransform, coordinateBoundaryTransform);
	EXPECT_TRUE(!composedBoundaryTransform.ReversesWinding);
	EXPECT_TRUE(composedBoundaryTransform.ReversesCameraFront);
	EXPECT_TRUE(composedBoundaryTransform.ReversesCulling);

	const Effekseer::SIMD::Vec3f point(3.0f, 5.0f, 7.0f);
	const auto composedPoint = Effekseer::SIMD::Vec3f::Transform(point, composedTransform.Transform);
	const auto sequentialPoint = Effekseer::SIMD::Vec3f::Transform(
		Effekseer::SIMD::Vec3f::Transform(point, effectTransform.Transform),
		reflectYTransform.Transform);
	EXPECT_EQUAL_NEAR(composedPoint.GetX(), sequentialPoint.GetX(), 0.0001f);
	EXPECT_EQUAL_NEAR(composedPoint.GetY(), sequentialPoint.GetY(), 0.0001f);
	EXPECT_EQUAL_NEAR(composedPoint.GetZ(), sequentialPoint.GetZ(), 0.0001f);

	class CaptureSpriteRenderer final : public Effekseer::SpriteRenderer
	{
	public:
		std::vector<Effekseer::EffectRenderingTransformParameter> Transforms;
		std::vector<Effekseer::EffectRenderingTransformParameter> CoordinateTransforms;

		void BeginRendering(const NodeParameter& parameter, int32_t count, void* userData) override
		{
			Transforms.emplace_back(parameter.RenderingTransform);
			CoordinateTransforms.emplace_back(parameter.RenderingCoordinateTransform);
		}
	};

	auto manager = Effekseer::Manager::Create(256);
	auto spriteRenderer = Effekseer::MakeRefPtr<CaptureSpriteRenderer>();
	manager->SetSpriteRenderer(spriteRenderer);
	const auto rootPath = GetDirectoryPathAsU16(__FILE__) + u"../../../../";
	auto effect = Effekseer::Effect::Create(manager, (rootPath + u"TestData/Effects/Update_17x/Sprite.efkefc").c_str());
	EXPECT_TRUE(effect != nullptr);

	Effekseer::Manager::PlayParameter playParameter;
	playParameter.Effect = effect;
	playParameter.Position = {10.0f, 20.0f, 30.0f};
	playParameter.Rotation = {0.2f, 0.4f, -0.3f};
	playParameter.Scale = {2.0f, 3.0f, 4.0f};
	playParameter.Flip = {true, false, false};
	const auto handle = manager->Play(playParameter);
	EXPECT_TRUE(handle >= 0);
	Effekseer::Matrix43 baseMatrix;
	Effekseer::Matrix43 baseRotation;
	baseRotation.RotationZ(0.6f);
	baseMatrix.SetSRT(
		{1.0f, 3.0f, 2.0f},
		baseRotation,
		{-2.0f, 1.0f, 4.0f});
	manager->SetBaseMatrix(handle, baseMatrix);
	manager->Update(0.0f);

	Effekseer::Manager::DrawParameter drawParameter;
	manager->DrawHandle(handle, drawParameter);
	EXPECT_TRUE(!spriteRenderer->Transforms.empty());
	const auto firstDrawTransform = spriteRenderer->Transforms.front();
	const auto firstCoordinateTransform = spriteRenderer->CoordinateTransforms.front();
	spriteRenderer->Transforms.clear();
	spriteRenderer->CoordinateTransforms.clear();

	drawParameter.RenderingCoordinateMatrix = reflectY;
	manager->DrawHandle(handle, drawParameter);
	EXPECT_TRUE(!spriteRenderer->Transforms.empty());
	const auto reflectedDrawTransform = spriteRenderer->Transforms.front();
	const auto reflectedCoordinateTransform = spriteRenderer->CoordinateTransforms.front();
	spriteRenderer->Transforms.clear();
	spriteRenderer->CoordinateTransforms.clear();

	drawParameter.RenderingCoordinateMatrix = identity;
	manager->DrawHandle(handle, drawParameter);
	EXPECT_TRUE(!spriteRenderer->Transforms.empty());
	const auto secondIdentityDrawTransform = spriteRenderer->Transforms.front();
	const auto secondIdentityCoordinateTransform = spriteRenderer->CoordinateTransforms.front();

	EXPECT_TRUE(firstDrawTransform.ReversesWinding);
	EXPECT_TRUE(firstDrawTransform.ReversesCulling);
	EXPECT_TRUE(!firstCoordinateTransform.IsEnabled);
	ExpectOrthogonalLinearPart(firstDrawTransform.Transform);
	EXPECT_TRUE(!reflectedDrawTransform.ReversesWinding);
	EXPECT_TRUE(!reflectedDrawTransform.ReversesCulling);
	EXPECT_TRUE(reflectedCoordinateTransform.IsEnabled);
	EXPECT_TRUE(reflectedCoordinateTransform.ReversesWinding);
	EXPECT_TRUE(reflectedCoordinateTransform.ReversesCulling);
	EXPECT_TRUE(Effekseer::SIMD::Mat43f::Equal(
		reflectYTransform.Transform,
		reflectedCoordinateTransform.Transform));
	EXPECT_TRUE(secondIdentityDrawTransform.ReversesWinding);
	EXPECT_TRUE(secondIdentityDrawTransform.ReversesCulling);
	EXPECT_TRUE(!secondIdentityCoordinateTransform.IsEnabled);
	EXPECT_TRUE(Effekseer::SIMD::Mat43f::Equal(
		firstDrawTransform.Transform,
		secondIdentityDrawTransform.Transform));
	EXPECT_TRUE(Effekseer::SIMD::Mat43f::Equal(
		firstDrawTransform.Transform * reflectYTransform.Transform,
		reflectedDrawTransform.Transform));
	manager->StopAllEffects();
}

void TestMirroredMatrixInterpolation()
{
	// A negative scale is a valid authoring parameter, so a mirrored matrix must be
	// interpolated between frames without losing the reflection.
	const auto makeMirroredMatrix = [](float angle) -> Effekseer::SIMD::Mat43f
	{
		return Effekseer::SIMD::Mat43f::SRT(
			Effekseer::SIMD::Vec3f(1.0f, -1.0f, 1.0f),
			Effekseer::SIMD::Mat43f::RotationZ(angle),
			Effekseer::SIMD::Vec3f(1.0f, 2.0f, 3.0f));
	};

	{
		const auto mirrored = makeMirroredMatrix(0.3f);
		Effekseer::TimeSeriesMatrix timeSeries;
		timeSeries.Reset(mirrored, 0.0f);
		timeSeries.Step(mirrored, 1.0f);
		EXPECT_TRUE(Effekseer::SIMD::Mat43f::Equal(timeSeries.Get(0.5f), mirrored, 0.001f));
	}

	{
		Effekseer::TimeSeriesMatrix timeSeries;
		timeSeries.Reset(makeMirroredMatrix(0.0f), 0.0f);
		timeSeries.Step(makeMirroredMatrix(0.8f), 1.0f);
		EXPECT_TRUE(Effekseer::SIMD::Mat43f::Equal(timeSeries.Get(0.5f), makeMirroredMatrix(0.4f), 0.001f));
	}
}

TestRegister RenderingTransform_TestEffectFlip("RenderingTransform.TestEffectFlip", []() -> void
											 { TestEffectFlip(); });

TestRegister RenderingTransform_TestMirroredMatrixInterpolation("RenderingTransform.TestMirroredMatrixInterpolation", []() -> void
																{ TestMirroredMatrixInterpolation(); });

TestRegister RenderingTransform_TestRenderingCoordinateTransform("RenderingTransform.TestRenderingCoordinateTransform", []() -> void
																	 { TestRenderingCoordinateTransform(); });

} // namespace
