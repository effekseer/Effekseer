#include "Effekseer.h"
#include "Effekseer/Effekseer.ManagerImplemented.h"

#include "../TestHelper.h"

#include <array>
#include <vector>

namespace
{

void ExpectVectorNear(const Effekseer::Vector3D& actual, const Effekseer::Vector3D& expected)
{
	EXPECT_EQUAL_NEAR(actual.X, expected.X, 0.0001f);
	EXPECT_EQUAL_NEAR(actual.Y, expected.Y, 0.0001f);
	EXPECT_EQUAL_NEAR(actual.Z, expected.Z, 0.0001f);
}

void ExpectMatrixNear(const Effekseer::Matrix43& actual, const Effekseer::Matrix43& expected)
{
	for (int32_t row = 0; row < 4; row++)
	{
		for (int32_t column = 0; column < 3; column++)
		{
			EXPECT_EQUAL_NEAR(actual.Value[row][column], expected.Value[row][column], 0.0001f);
		}
	}
}

void ExpectMatrixNear(const Effekseer::Matrix44& actual, const Effekseer::Matrix44& expected)
{
	for (int32_t row = 0; row < 4; row++)
	{
		for (int32_t column = 0; column < 4; column++)
		{
			EXPECT_EQUAL_NEAR(actual.Values[row][column], expected.Values[row][column], 0.0001f);
		}
	}
}

Effekseer::Matrix44 MakeAxisExchangeReflection()
{
	// internal (x, y, z) -> external (z, x, -y)
	Effekseer::Matrix44 result;
	for (int32_t row = 0; row < 3; row++)
	{
		for (int32_t column = 0; column < 3; column++)
		{
			result.Values[row][column] = 0.0f;
		}
	}
	result.Values[0][1] = 1.0f;
	result.Values[1][2] = -1.0f;
	result.Values[2][0] = 1.0f;
	return result;
}

void TestCoordinateSystemConverter()
{
	const auto identity = Effekseer::CoordinateSystemConverter::FromCoordinateSystem(Effekseer::CoordinateSystem::RH);
	EXPECT_TRUE(identity.IsValid());
	EXPECT_TRUE(!identity.ReversesWinding());
	ExpectVectorNear(identity.ToInternalPosition({1.0f, 2.0f, 3.0f}), {1.0f, 2.0f, 3.0f});

	const auto leftHanded = Effekseer::CoordinateSystemConverter::FromCoordinateSystem(Effekseer::CoordinateSystem::LH);
	EXPECT_TRUE(leftHanded.IsValid());
	EXPECT_TRUE(leftHanded.ReversesWinding());
	ExpectVectorNear(leftHanded.ToExternalPosition({1.0f, 2.0f, 3.0f}), {1.0f, 2.0f, -3.0f});

	const auto coordinateMatrix = MakeAxisExchangeReflection();
	EXPECT_TRUE(Effekseer::IsValidCoordinateSystemMatrix(coordinateMatrix));
	const auto converter = Effekseer::CoordinateSystemConverter::FromMatrix(coordinateMatrix);
	EXPECT_TRUE(converter.IsValid());
	EXPECT_TRUE(converter.ReversesWinding());

	const Effekseer::Vector3D internalPosition = {1.0f, 2.0f, 3.0f};
	const Effekseer::Vector3D externalPosition = {3.0f, 1.0f, -2.0f};
	ExpectVectorNear(converter.ToExternalPosition(internalPosition), externalPosition);
	ExpectVectorNear(converter.ToInternalPosition(externalPosition), internalPosition);
	ExpectVectorNear(converter.ToInternalDirection({5.0f, 6.0f, 7.0f}), {6.0f, -7.0f, 5.0f});
	ExpectVectorNear(converter.ToInternalPseudoVector({5.0f, 6.0f, 7.0f}), {-6.0f, 7.0f, -5.0f});
	ExpectVectorNear(converter.ToInternalScale({2.0f, 3.0f, 4.0f}), {3.0f, 4.0f, 2.0f});
	ExpectVectorNear(converter.ToExternalScale({3.0f, 4.0f, 2.0f}), {2.0f, 3.0f, 4.0f});

	Effekseer::Matrix43 rotation;
	rotation.RotationZXY(0.4f, -0.2f, 0.7f);
	Effekseer::Matrix43 transform;
	transform.SetSRT({2.0f, 3.0f, 4.0f}, rotation, {5.0f, 6.0f, 7.0f});
	const auto internalTransform = converter.ToInternalTransform(transform);
	EXPECT_TRUE(internalTransform.IsProperSRT());
	ExpectMatrixNear(converter.ToExternalTransform(internalTransform), transform);

	Effekseer::Matrix44 externalView;
	externalView.LookAtRH({4.0f, 5.0f, 6.0f}, {1.0f, 2.0f, 3.0f}, {0.0f, 1.0f, 0.0f});
	const auto internalView = converter.ToInternalView(externalView);
	ExpectMatrixNear(converter.ToExternalView(internalView), externalView);

	Effekseer::Matrix44 invalidScale;
	invalidScale.Scaling(1.0f, 2.0f, 1.0f);
	EXPECT_TRUE(!Effekseer::IsValidCoordinateSystemMatrix(invalidScale));
	EXPECT_TRUE(!Effekseer::CoordinateSystemConverter::FromMatrix(invalidScale).IsValid());

	Effekseer::Matrix44 arbitraryRotation;
	arbitraryRotation.RotationY(0.25f);
	EXPECT_TRUE(Effekseer::IsValidRenderingCoordinateMatrix(arbitraryRotation));
	EXPECT_TRUE(!Effekseer::IsValidCoordinateSystemMatrix(arbitraryRotation));
}

void TestManagerCoordinateSystemBoundary()
{
	auto legacyManager = Effekseer::Manager::Create(16);
	EXPECT_TRUE(legacyManager->GetCoordinateSystemMode() == Effekseer::CoordinateSystemMode::LegacySimulation);
	legacyManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
	EXPECT_TRUE(legacyManager->GetCoordinateSystem() == Effekseer::CoordinateSystem::LH);
	EXPECT_TRUE(legacyManager->GetSetting()->GetCoordinateSystem() == Effekseer::CoordinateSystem::LH);
	legacyManager->GetSetting()->SetCoordinateSystem(Effekseer::CoordinateSystem::RH);
	EXPECT_TRUE(legacyManager->GetCoordinateSystem() == Effekseer::CoordinateSystem::RH);

	auto manager = Effekseer::Manager::Create(256);
	Effekseer::CoordinateSystemTransform coordinateTransform;
	coordinateTransform.ToExternal = MakeAxisExchangeReflection();
	EXPECT_TRUE(manager->SetCoordinateSystemTransform(coordinateTransform));
	EXPECT_TRUE(manager->GetCoordinateSystem() == Effekseer::CoordinateSystem::RH);
	EXPECT_TRUE(manager->GetSetting()->GetCoordinateSystem() == Effekseer::CoordinateSystem::RH);
	manager->SetCoordinateSystemMode(Effekseer::CoordinateSystemMode::ExternalConversion);
	EXPECT_TRUE(manager->GetCoordinateSystemMode() == Effekseer::CoordinateSystemMode::ExternalConversion);
	EXPECT_TRUE(manager->GetCoordinateSystem() == Effekseer::CoordinateSystem::LH);
	EXPECT_TRUE(manager->GetSetting()->GetCoordinateSystem() == Effekseer::CoordinateSystem::RH);
	ExpectMatrixNear(manager->GetCoordinateSystemTransform().ToExternal, coordinateTransform.ToExternal);

	Effekseer::CoordinateSystemTransform invalidTransform;
	invalidTransform.ToExternal.Scaling(1.0f, 2.0f, 1.0f);
	EXPECT_TRUE(!manager->SetCoordinateSystemTransform(invalidTransform));
	ExpectMatrixNear(manager->GetCoordinateSystemTransform().ToExternal, coordinateTransform.ToExternal);

	Effekseer::Manager::LayerParameter layerParameter;
	layerParameter.ViewerPosition = {3.0f, 1.0f, -2.0f};
	layerParameter.DistanceBias = 12.0f;
	manager->SetLayerParameter(2, layerParameter);
	ExpectVectorNear(manager->GetLayerParameter(2).ViewerPosition, layerParameter.ViewerPosition);
	auto implemented = manager->GetImplemented();
	ExpectVectorNear(implemented->GetInternalLayerParameter(2).ViewerPosition, {1.0f, 2.0f, 3.0f});
	EXPECT_EQUAL_NEAR(implemented->GetInternalLayerParameter(2).DistanceBias, 12.0f, 0.0001f);

	Effekseer::Vector3D callbackStart;
	Effekseer::Vector3D callbackEnd;
	manager->SetCollisionCallback([&](const Effekseer::Vector3D& start,
									 const Effekseer::Vector3D& end,
									 Effekseer::Vector3D& hit,
									 Effekseer::Vector3D& normal) -> bool
	{
		callbackStart = start;
		callbackEnd = end;
		hit = {5.0f, 6.0f, 7.0f};
		normal = {0.0f, 1.0f, 0.0f};
		return true;
	});
	Effekseer::Vector3D internalHit;
	Effekseer::Vector3D internalNormal;
	EXPECT_TRUE(implemented->GetInternalCollisionCallback()(
		{1.0f, 2.0f, 3.0f}, {4.0f, 5.0f, 6.0f}, internalHit, internalNormal));
	ExpectVectorNear(callbackStart, {3.0f, 1.0f, -2.0f});
	ExpectVectorNear(callbackEnd, {6.0f, 4.0f, -5.0f});
	ExpectVectorNear(internalHit, {6.0f, -7.0f, 5.0f});
	ExpectVectorNear(internalNormal, {1.0f, 0.0f, 0.0f});

	const auto rootPath = GetDirectoryPathAsU16(__FILE__) + u"../../../../";
	auto effect = Effekseer::Effect::Create(manager, (rootPath + u"TestData/Effects/Update_17x/Sprite.efkefc").c_str());
	EXPECT_TRUE(effect != nullptr);
	EXPECT_TRUE(effect->GetSetting()->GetCoordinateSystem() == Effekseer::CoordinateSystem::RH);

	Effekseer::Manager::PlayParameter playParameter;
	playParameter.Effect = effect;
	playParameter.Position = {3.0f, 1.0f, -2.0f};
	playParameter.Rotation = {0.2f, -0.4f, 0.7f};
	playParameter.Scale = {2.0f, 3.0f, 4.0f};
	const auto handle = manager->Play(playParameter);
	EXPECT_TRUE(handle >= 0);
	Effekseer::Matrix43 expectedRotation;
	expectedRotation.RotationZXY(playParameter.Rotation.Z, playParameter.Rotation.X, playParameter.Rotation.Y);
	Effekseer::Matrix43 expectedMatrix;
	expectedMatrix.SetSRT(playParameter.Scale, expectedRotation, playParameter.Position);
	ExpectMatrixNear(manager->GetMatrix(handle), expectedMatrix);

	Effekseer::Matrix43 replacementRotation;
	replacementRotation.RotationZXY(-0.3f, 0.6f, 0.1f);
	Effekseer::Matrix43 replacementMatrix;
	replacementMatrix.SetSRT({5.0f, 6.0f, 7.0f}, replacementRotation, {-8.0f, 9.0f, 10.0f});
	manager->SetMatrix(handle, replacementMatrix);
	ExpectMatrixNear(manager->GetMatrix(handle), replacementMatrix);
	manager->SetLocation(handle, {2.0f, 4.0f, 6.0f});
	manager->AddLocation(handle, {-1.0f, 3.0f, 2.0f});
	ExpectVectorNear(manager->GetLocation(handle), {1.0f, 7.0f, 8.0f});

	Effekseer::Matrix43 setterRotation;
	setterRotation.RotationZXY(0.5f, -0.25f, 0.75f);
	manager->SetRotation(handle, -0.25f, 0.75f, 0.5f);
	Effekseer::Matrix43 expectedAfterSetters;
	expectedAfterSetters.SetSRT({5.0f, 6.0f, 7.0f}, setterRotation, {1.0f, 7.0f, 8.0f});
	ExpectMatrixNear(manager->GetMatrix(handle), expectedAfterSetters);
	manager->SetScale(handle, 9.0f, 8.0f, 7.0f);
	expectedAfterSetters.SetSRT({9.0f, 8.0f, 7.0f}, setterRotation, {1.0f, 7.0f, 8.0f});
	ExpectMatrixNear(manager->GetMatrix(handle), expectedAfterSetters);
	const Effekseer::Vector3D rotationAxis = {0.0f, 1.0f, 0.0f};
	setterRotation.RotationAxis(rotationAxis, 0.35f);
	manager->SetRotation(handle, rotationAxis, 0.35f);
	expectedAfterSetters.SetSRT({9.0f, 8.0f, 7.0f}, setterRotation, {1.0f, 7.0f, 8.0f});
	ExpectMatrixNear(manager->GetMatrix(handle), expectedAfterSetters);

	manager->SetBaseMatrix(handle, expectedMatrix);
	ExpectMatrixNear(manager->GetBaseMatrix(handle), expectedMatrix);
	manager->StopAllEffects();

	manager->SetCoordinateSystemMode(Effekseer::CoordinateSystemMode::LegacySimulation);
	EXPECT_TRUE(manager->GetSetting()->GetCoordinateSystem() == Effekseer::CoordinateSystem::LH);
	manager->SetCoordinateSystemMode(Effekseer::CoordinateSystemMode::ExternalConversion);
	EXPECT_TRUE(manager->GetSetting()->GetCoordinateSystem() == Effekseer::CoordinateSystem::RH);
	ExpectMatrixNear(manager->GetCoordinateSystemTransform().ToExternal, coordinateTransform.ToExternal);
}

void TestManagerAutomaticRenderingTransform()
{
	class CaptureSpriteRenderer final : public Effekseer::SpriteRenderer
	{
	public:
		std::vector<NodeParameter> Parameters;
		std::vector<Effekseer::Vector3D> OutputCenters;

		void BeginRendering(const NodeParameter& parameter, int32_t count, void* userData) override
		{
			Parameters.emplace_back(parameter);
		}

		void Rendering(const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData) override
		{
			auto center = instanceParameter.SRTMatrix43.GetTranslation();
			if (parameter.RenderingTransform.IsEnabled)
			{
				center = Effekseer::SIMD::Vec3f::Transform(center, parameter.RenderingTransform.Transform);
			}
			OutputCenters.emplace_back(Effekseer::SIMD::ToStruct(center));
		}
	};

	auto manager = Effekseer::Manager::Create(256);
	manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
	manager->SetCoordinateSystemMode(Effekseer::CoordinateSystemMode::ExternalConversion);
	EXPECT_TRUE(manager->GetSetting()->GetCoordinateSystem() == Effekseer::CoordinateSystem::RH);

	auto spriteRenderer = Effekseer::MakeRefPtr<CaptureSpriteRenderer>();
	manager->SetSpriteRenderer(spriteRenderer);
	const auto rootPath = GetDirectoryPathAsU16(__FILE__) + u"../../../../";
	auto effect = Effekseer::Effect::Create(manager, (rootPath + u"TestData/Effects/Update_17x/Sprite.efkefc").c_str());
	EXPECT_TRUE(effect != nullptr);
	const auto handle = manager->Play(effect, {1.0f, 2.0f, 3.0f});
	EXPECT_TRUE(handle >= 0);
	manager->Update(0.0f);

	Effekseer::Manager::DrawParameter drawParameter;
	manager->DrawHandle(handle, drawParameter);
	EXPECT_TRUE(!spriteRenderer->Parameters.empty());
	const auto automaticParameter = spriteRenderer->Parameters.back();
	EXPECT_TRUE(automaticParameter.IsRightHand);
	EXPECT_TRUE(automaticParameter.RenderingCoordinateTransform.IsEnabled);
	EXPECT_TRUE(automaticParameter.RenderingCoordinateTransform.ReversesWinding);
	EXPECT_TRUE(automaticParameter.RenderingCoordinateTransform.ReversesCameraFront);
	EXPECT_TRUE(!automaticParameter.RenderingCoordinateTransform.ReversesCulling);
	EXPECT_TRUE(automaticParameter.RenderingTransform.ReversesCameraFront);
	EXPECT_TRUE(!automaticParameter.RenderingTransform.ReversesCulling);
	Effekseer::Matrix44 reflectZ;
	reflectZ.Scaling(1.0f, 1.0f, -1.0f);
	EXPECT_TRUE(Effekseer::SIMD::Mat43f::Equal(
		automaticParameter.RenderingCoordinateTransform.Transform,
		Effekseer::CalculateRenderingCoordinateTransform(reflectZ).Transform));

	spriteRenderer->Parameters.clear();
	drawParameter.RenderingCoordinateMatrix.Scaling(1.0f, -1.0f, 1.0f);
	manager->DrawHandle(handle, drawParameter);
	EXPECT_TRUE(!spriteRenderer->Parameters.empty());
	const auto composedParameter = spriteRenderer->Parameters.back();
	EXPECT_TRUE(composedParameter.RenderingCoordinateTransform.IsEnabled);
	EXPECT_TRUE(!composedParameter.RenderingCoordinateTransform.ReversesWinding);
	EXPECT_TRUE(composedParameter.RenderingCoordinateTransform.ReversesCameraFront);
	EXPECT_TRUE(composedParameter.RenderingCoordinateTransform.ReversesCulling);
	EXPECT_TRUE(composedParameter.RenderingTransform.ReversesCameraFront);
	EXPECT_TRUE(composedParameter.RenderingTransform.ReversesCulling);
	const auto expectedComposed = Effekseer::ComposeRenderingTransforms(
		Effekseer::CalculateRenderingCoordinateTransform(reflectZ),
		Effekseer::CalculateRenderingCoordinateTransform(drawParameter.RenderingCoordinateMatrix));
	EXPECT_TRUE(Effekseer::SIMD::Mat43f::Equal(
		composedParameter.RenderingCoordinateTransform.Transform,
		expectedComposed.Transform));

	manager->SetEffectFlip(handle, {true, false, false});
	manager->Update(0.0f);
	spriteRenderer->Parameters.clear();
	drawParameter.RenderingCoordinateMatrix.Indentity();
	manager->DrawHandle(handle, drawParameter);
	EXPECT_TRUE(!spriteRenderer->Parameters.empty());
	const auto flippedBoundaryParameter = spriteRenderer->Parameters.back();
	EXPECT_TRUE(!flippedBoundaryParameter.RenderingTransform.ReversesWinding);
	EXPECT_TRUE(flippedBoundaryParameter.RenderingTransform.ReversesCameraFront);
	EXPECT_TRUE(flippedBoundaryParameter.RenderingTransform.ReversesCulling);

	spriteRenderer->Parameters.clear();
	drawParameter.RenderingCoordinateMatrix.Scaling(1.0f, -1.0f, 1.0f);
	manager->DrawHandle(handle, drawParameter);
	EXPECT_TRUE(!spriteRenderer->Parameters.empty());
	const auto flippedComposedParameter = spriteRenderer->Parameters.back();
	EXPECT_TRUE(flippedComposedParameter.RenderingTransform.ReversesWinding);
	EXPECT_TRUE(flippedComposedParameter.RenderingTransform.ReversesCameraFront);
	EXPECT_TRUE(!flippedComposedParameter.RenderingTransform.ReversesCulling);
	manager->StopAllEffects();

	const auto renderLegacyComparison = [&](bool externalConversion) -> std::vector<Effekseer::Vector3D>
	{
		auto comparisonManager = Effekseer::Manager::Create(256);
		comparisonManager->SetRandFunc([]() -> int { return 12345; });
		comparisonManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
		if (externalConversion)
		{
			comparisonManager->SetCoordinateSystemMode(Effekseer::CoordinateSystemMode::ExternalConversion);
		}
		auto comparisonRenderer = Effekseer::MakeRefPtr<CaptureSpriteRenderer>();
		comparisonManager->SetSpriteRenderer(comparisonRenderer);
		auto comparisonEffect = Effekseer::Effect::Create(
			comparisonManager,
			(rootPath + u"TestData/Effects/Update_17x/Sprite.efkefc").c_str());
		EXPECT_TRUE(comparisonEffect != nullptr);
		const auto comparisonHandle = comparisonManager->Play(comparisonEffect, {10.0f, 20.0f, 30.0f});
		EXPECT_TRUE(comparisonHandle >= 0);
		for (int32_t frame = 0; frame < 10; frame++)
		{
			comparisonManager->Update(1.0f);
		}
		comparisonManager->DrawHandle(comparisonHandle, {});
		comparisonManager->StopAllEffects();
		return comparisonRenderer->OutputCenters;
	};

	const auto legacyCenters = renderLegacyComparison(false);
	const auto externalConversionCenters = renderLegacyComparison(true);
	EXPECT_TRUE(!legacyCenters.empty());
	EXPECT_TRUE(legacyCenters.size() == externalConversionCenters.size());
	for (size_t index = 0; index < legacyCenters.size(); index++)
	{
		ExpectVectorNear(externalConversionCenters[index], legacyCenters[index]);
	}
}

void TestModelBoundaryCulling()
{
	class CaptureModelRenderer final : public Effekseer::ModelRenderer
	{
	public:
		std::vector<NodeParameter> Parameters;

		void BeginRendering(const NodeParameter& parameter, int32_t count, void* userData) override
		{
			Parameters.emplace_back(parameter);
		}
	};

	struct TestCase
	{
		const char16_t* Path;
		int32_t FrameCount;
	};

	const std::array<TestCase, 9> testCases = {{
		{u"TestData/Effects/14/Model_Parameters1.efk", 30},
		{u"TestData/Effects/15/Model_Culling.efkefc", 30},
		{u"TestData/Effects/15/Update_MultiModel.efkefc", 30},
		{u"TestData/Effects/16/AnimatedModel01.efkefc", 30},
		{u"TestData/Effects/16/DrawWithoutInstancing.efkefc", 30},
		{u"TestData/Effects/16/ProcedualModel01.efkefc", 30},
		{u"TestData/Effects/16/ProcedualModel02.efkefc", 30},
		{u"TestData/Effects/16/ProcedualModel03.efkefc", 30},
		{u"TestData/Effects/Update_17x/Model.efkefc", 30},
	}};

	const auto rootPath = GetDirectoryPathAsU16(__FILE__) + u"../../../../";
	int32_t cullingTypes = 0;
	bool sawProceduralModel = false;
	bool sawRegularModel = false;

	for (const auto& testCase : testCases)
	{
		auto manager = Effekseer::Manager::Create(2048);
		manager->SetRandFunc([]() -> int { return 12345; });
		manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
		manager->SetCoordinateSystemMode(Effekseer::CoordinateSystemMode::ExternalConversion);
		auto renderer = Effekseer::MakeRefPtr<CaptureModelRenderer>();
		manager->SetModelRenderer(renderer);

		auto effect = Effekseer::Effect::Create(manager, (rootPath + testCase.Path).c_str());
		EXPECT_TRUE(effect != nullptr);
		const auto handle = manager->Play(effect, {2.0f, 0.0f, 1.0f});
		EXPECT_TRUE(handle >= 0);
		manager->SetRotation(handle, 0.15f, 0.45f, -0.2f);
		manager->SetScale(handle, 1.1f, 0.8f, 0.7f);
		for (int32_t frame = 0; frame < testCase.FrameCount; frame++)
		{
			manager->Update();
		}

		manager->DrawHandle(handle, {});
		EXPECT_TRUE(!renderer->Parameters.empty());
		for (const auto& parameter : renderer->Parameters)
		{
			EXPECT_TRUE(parameter.RenderingCoordinateTransform.ReversesWinding);
			EXPECT_TRUE(parameter.RenderingCoordinateTransform.ReversesCameraFront);
			EXPECT_TRUE(!parameter.RenderingCoordinateTransform.ReversesCulling);
			EXPECT_TRUE(parameter.RenderingTransform.ReversesCameraFront);
			EXPECT_TRUE(!parameter.RenderingTransform.ReversesCulling);
			EXPECT_TRUE(Effekseer::GetTransformedCullingType(parameter.Culling, parameter.RenderingTransform) == parameter.Culling);
			cullingTypes |= 1 << static_cast<int32_t>(parameter.Culling);
			sawProceduralModel |= parameter.IsProceduralMode;
			sawRegularModel |= !parameter.IsProceduralMode;
		}

		manager->SetEffectFlip(handle, {true, false, false});
		manager->Update(0.0f);
		renderer->Parameters.clear();
		manager->DrawHandle(handle, {});
		EXPECT_TRUE(!renderer->Parameters.empty());
		for (const auto& parameter : renderer->Parameters)
		{
			EXPECT_TRUE(parameter.RenderingTransform.ReversesCameraFront);
			EXPECT_TRUE(parameter.RenderingTransform.ReversesCulling);
			const auto expected = parameter.Culling == Effekseer::CullingType::Front
				? Effekseer::CullingType::Back
				: parameter.Culling == Effekseer::CullingType::Back
					? Effekseer::CullingType::Front
					: Effekseer::CullingType::Double;
			EXPECT_TRUE(Effekseer::GetTransformedCullingType(parameter.Culling, parameter.RenderingTransform) == expected);
		}
	}

	EXPECT_TRUE(cullingTypes == 0b111);
	EXPECT_TRUE(sawProceduralModel);
	EXPECT_TRUE(sawRegularModel);
}

TestRegister CoordinateSystem_Converter(
	"CoordinateSystem.Converter",
	[]() -> void { TestCoordinateSystemConverter(); });

TestRegister CoordinateSystem_ManagerBoundary(
	"CoordinateSystem.ManagerBoundary",
	[]() -> void { TestManagerCoordinateSystemBoundary(); });

TestRegister CoordinateSystem_AutomaticRenderingTransform(
	"CoordinateSystem.AutomaticRenderingTransform",
	[]() -> void { TestManagerAutomaticRenderingTransform(); });

TestRegister CoordinateSystem_ModelBoundaryCulling(
	"CoordinateSystem.ModelBoundaryCulling",
	[]() -> void { TestModelBoundaryCulling(); });

} // namespace
