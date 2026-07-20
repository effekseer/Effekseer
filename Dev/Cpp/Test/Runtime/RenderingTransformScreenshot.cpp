#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifdef _WIN32
#include <Runtime/EffectPlatformDX11.h>
#endif

#include <Runtime/EffectPlatform.h>

#include "../../Effekseer/Effekseer/Effekseer.EffectNode.h"

#include "../TestHelper.h"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

namespace
{

#ifdef _WIN32

struct RenderingTransformScreenshotCase
{
	const char* Name;
	const char16_t* EffectPath;
	int32_t FrameCount;
	float FrontOrthographicHeight;
	float XZOrthographicHeight;
	bool UseLightingVariants;
	bool ForceGpuLighting;
};

struct RenderingTransformScreenshotVariant
{
	const char* Label;
	Effekseer::EffectFlipParameter Flip;
	enum class RenderingCoordinateTransform
	{
		Identity,
		ReflectY,
		ReflectZ,
		TurnZ,
		TurnY,
		TiltX,
	} CoordinateTransform;
};

const std::array<RenderingTransformScreenshotCase, 11> ScreenshotCases = {{
	{"Sprite", u"TestData/Effects/Update_17x/Sprite.efkefc", 30, 12.0f, 0.0f, false, false},
	{"SpriteLighting", u"TestData/Effects/15/Material_Lighting.efkefc", 30, 12.0f, 0.0f, true, false},
	{"SpriteNormalMap", u"TestData/Effects/15/Material_Normal.efkefc", 30, 12.0f, 0.0f, true, false},
	// This ribbon lies on the XZ plane and is edge-on in the XY front view.
	{"Ribbon", u"TestData/Effects/10/Ribbon_Parameters1.efk", 30, 0.0f, 20.0f, false, false},
	{"Ring", u"TestData/Effects/10/Ring_Parameters1.efk", 30, 28.0f, 28.0f, false, false},
	{"Track", u"TestData/Effects/Update_17x/Track.efkefc", 30, 20.0f, 0.0f, false, false},
	{"Model", u"TestData/Effects/Update_17x/Model.efkefc", 30, 24.0f, 24.0f, false, false},
	{"GpuParticlesSprite", u"TestData/Effects/18/GpuParticles_sprite_simple.efkefc", 120, 24.0f, 24.0f, true, false},
	{"GpuParticlesTrail", u"TestData/Effects/18/GpuParticles_trails_simple.efkefc", 120, 24.0f, 24.0f, false, false},
	{"GpuParticlesMesh", u"TestData/Effects/18/GpuParticles_emit_mesh.efkefc", 120, 24.0f, 24.0f, false, false},
	{"GpuParticlesLighting", u"TestData/Effects/18/GpuParticles_sprite_simple.efkefc", 120, 24.0f, 0.0f, true, true},
}};

const std::array<RenderingTransformScreenshotVariant, 6> ScreenshotVariants = {{
	{"UNCHANGED", {}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::Identity},
	{"FLIP-X", {true, false, false}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::Identity},
	{"FLIP-Y", {false, true, false}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::Identity},
	{"REFLECT-Y", {}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::ReflectY},
	{"TURN-Z", {}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::TurnZ},
	{"TILT-X", {}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::TiltX},
}};

const std::array<RenderingTransformScreenshotVariant, 6> LightingScreenshotVariants = {{
	{"UNCHANGED", {}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::Identity},
	{"FLIP-X", {true, false, false}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::Identity},
	{"FLIP-Y", {false, true, false}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::Identity},
	{"REFLECT-Y", {}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::ReflectY},
	{"TURN-Z", {}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::TurnZ},
	{"TILT-X", {}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::TiltX},
}};

const std::array<RenderingTransformScreenshotVariant, 6> XZScreenshotVariants = {{
	{"UNCHANGED", {}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::Identity},
	{"FLIP-X", {true, false, false}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::Identity},
	{"FLIP-Z", {false, false, true}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::Identity},
	{"REFLECT-Z", {}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::ReflectZ},
	{"TURN-Y", {}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::TurnY},
	{"FLIP-Z+REFLECT-Z", {false, false, true}, RenderingTransformScreenshotVariant::RenderingCoordinateTransform::ReflectZ},
}};

Effekseer::Matrix44 MakeRenderingCoordinateMatrix(RenderingTransformScreenshotVariant::RenderingCoordinateTransform transform)
{
	Effekseer::Matrix44 matrix;
	switch (transform)
	{
	case RenderingTransformScreenshotVariant::RenderingCoordinateTransform::ReflectY:
		matrix.Scaling(1.0f, -1.0f, 1.0f);
		break;
	case RenderingTransformScreenshotVariant::RenderingCoordinateTransform::ReflectZ:
		matrix.Scaling(1.0f, 1.0f, -1.0f);
		break;
	case RenderingTransformScreenshotVariant::RenderingCoordinateTransform::TurnZ:
		matrix.RotationZ(3.14159265f / 2.0f);
		break;
	case RenderingTransformScreenshotVariant::RenderingCoordinateTransform::TurnY:
		matrix.RotationY(3.14159265f / 2.0f);
		break;
	case RenderingTransformScreenshotVariant::RenderingCoordinateTransform::TiltX:
		matrix.RotationX(3.14159265f / 4.0f);
		break;
	case RenderingTransformScreenshotVariant::RenderingCoordinateTransform::Identity:
		break;
	}
	return matrix;
}

void ForceGpuParticleLighting(
	const Effekseer::ManagerRef& manager,
	const Effekseer::EffectRef& effect)
{
	auto* rootNode = static_cast<Effekseer::EffectNodeImplemented*>(effect->GetRoot());
	bool resourceFound = false;
	rootNode->Traverse(
		[&](Effekseer::EffectNodeImplemented* node)
		{
			if (node->GpuParticlesResource != nullptr)
			{
				auto paramSet = node->GpuParticlesResource->GetParamSet();
				paramSet.RenderMaterial.Material = Effekseer::GpuParticles::MaterialType::Lighting;
				node->GpuParticlesResource = manager->GetGpuParticleFactory()->CreateResource(paramSet, effect.Get());
				resourceFound = true;
			}
			return true;
		});
	EXPECT_TRUE(resourceFound);
}

void LogGpuParticleParameters(const Effekseer::EffectRef& effect, const char* caseName)
{
	auto* rootNode = static_cast<Effekseer::EffectNodeImplemented*>(effect->GetRoot());
	rootNode->Traverse(
		[caseName](Effekseer::EffectNodeImplemented* node)
		{
			if (node->GpuParticlesResource != nullptr)
			{
				const auto& paramSet = node->GpuParticlesResource->GetParamSet();
				printf("[RenderingTransformScreenshot] case=%s gpu-shape=%d gpu-material=%d\n",
					caseName,
					static_cast<int32_t>(paramSet.RenderShape.Type),
					static_cast<int32_t>(paramSet.RenderMaterial.Material));
			}
			return true;
		});
}

enum class OrthographicView
{
	FrontXY,
	XZ,
};

void ConfigureOrthographicCamera(EffectPlatform& platform, OrthographicView view, float orthographicHeight)
{
	constexpr float AspectRatio = 4.0f / 3.0f;
	const auto cameraPosition = view == OrthographicView::FrontXY
		? Effekseer::Vector3D(0.0f, 0.0f, 10.0f)
		: Effekseer::Vector3D(0.0f, -10.0f, 0.0f);
	const auto cameraTarget = Effekseer::Vector3D(0.0f, 0.0f, 0.0f);
	const auto cameraUp = view == OrthographicView::FrontXY
		? Effekseer::Vector3D(0.0f, 1.0f, 0.0f)
		: Effekseer::Vector3D(0.0f, 0.0f, 1.0f);

	platform.GetRenderer()->SetCameraMatrix(
		Effekseer::Matrix44().LookAtRH(cameraPosition, cameraTarget, cameraUp));
	platform.GetRenderer()->SetProjectionMatrix(
		Effekseer::Matrix44().OrthographicRH(orthographicHeight * AspectRatio, orthographicHeight, 1.0f, 50.0f));
}

void CaptureOrthographicComparisons(
	EffectPlatform& platform,
	std::ofstream& manifest,
	const std::filesystem::path& sourceRoot,
	const std::u16string& rootPath,
	OrthographicView view)
{
	const bool isFrontXY = view == OrthographicView::FrontXY;
	const char* viewName = isFrontXY ? "FrontOrthographic" : "XZOrthographic";
	const char* compositePrefix = isFrontXY
		? "RenderingTransformFrontOrthographicComparison_"
		: "RenderingTransformXZOrthographicComparison_";

	for (const auto& screenshotCase : ScreenshotCases)
	{
		const float orthographicHeight = isFrontXY
			? screenshotCase.FrontOrthographicHeight
			: screenshotCase.XZOrthographicHeight;
		if (orthographicHeight <= 0.0f)
		{
			printf("[RenderingTransformScreenshot] view=%s case=%s skipped\n", viewName, screenshotCase.Name);
			continue;
		}
		ConfigureOrthographicCamera(platform, view, orthographicHeight);

		printf("[RenderingTransformScreenshot] view=%s case=%s frames=%d\n",
			viewName,
			screenshotCase.Name,
			screenshotCase.FrameCount);
		const auto caseDirectory = sourceRoot / "DX11" / viewName / screenshotCase.Name;
		std::filesystem::create_directories(caseDirectory);

		srand(0);
		Effekseer::EffectRef effect;
		Effekseer::Handle handle;
		if (screenshotCase.ForceGpuLighting)
		{
			effect = Effekseer::Effect::Create(platform.GetManager(), (rootPath + screenshotCase.EffectPath).c_str());
			EXPECT_TRUE(effect != nullptr);
			ForceGpuParticleLighting(platform.GetManager(), effect);
			handle = platform.GetManager()->Play(
				effect,
				isFrontXY ? Effekseer::Vector3D(2.0f, 1.0f, 0.0f) : Effekseer::Vector3D(2.0f, 0.0f, 1.0f));
			platform.GetRenderer()->SetLightDirection({0.0f, -0.70710678f, 0.70710678f});
		}
		else
		{
			handle = platform.Play(
				(rootPath + screenshotCase.EffectPath).c_str(),
				isFrontXY ? Effekseer::Vector3D(2.0f, 1.0f, 0.0f) : Effekseer::Vector3D(2.0f, 0.0f, 1.0f));
			EXPECT_TRUE(!platform.GetEffects().empty());
			effect = platform.GetEffects().back();
		}
		EXPECT_TRUE(handle >= 0);
		LogGpuParticleParameters(effect, screenshotCase.Name);

		for (int32_t frame = 0; frame < screenshotCase.FrameCount; frame++)
		{
			EXPECT_TRUE(platform.Update());
		}

		const auto& variants = !isFrontXY
			? XZScreenshotVariants
			: (screenshotCase.UseLightingVariants ? LightingScreenshotVariants : ScreenshotVariants);
		for (size_t variantIndex = 0; variantIndex < variants.size(); variantIndex++)
		{
			const auto& variant = variants[variantIndex];
			printf("[RenderingTransformScreenshot] view=%s case=%s variant=%s\n",
				viewName,
				screenshotCase.Name,
				variant.Label);
			platform.GetManager()->SetEffectFlip(handle, variant.Flip);
			EXPECT_TRUE(platform.GetManager()->GetEffectFlip(handle) == variant.Flip);
			platform.GetManager()->Flip();
			platform.SetRenderingCoordinateMatrix(MakeRenderingCoordinateMatrix(variant.CoordinateTransform));
			EXPECT_TRUE(platform.Draw());

			const auto sourcePath = caseDirectory /
				(std::to_string(variantIndex) + "_" + variant.Label + ".png");
			EXPECT_TRUE(platform.TakeScreenshot(sourcePath.string().c_str()));

			manifest << compositePrefix << screenshotCase.Name << "_DX11.png,"
					 << variantIndex << "," << variant.Label << "," << sourcePath.generic_string() << "\n";
		}

		platform.SetRenderingCoordinateMatrix(Effekseer::Matrix44());
		platform.GetRenderer()->SetLightDirection({1.0f, 1.0f, 1.0f});
		platform.StopAllEffects();
		platform.ClearLoadedEffects();
	}
}

void RunRenderingTransformScreenshotTest()
{
	const std::filesystem::path sourceRoot = "screenshot_comparison_sources";
	const auto manifestPath = sourceRoot / "manifest.csv";
	std::filesystem::create_directories(sourceRoot);

	std::ofstream manifest(manifestPath, std::ios::trunc);
	EXPECT_TRUE(manifest.is_open());
	manifest << "composite,order,label,source\n";

	const auto rootPath = GetDirectoryPathAsU16(__FILE__) + u"../../../../";

	EffectPlatformInitializingParameter initializingParameter;
	initializingParameter.BackgroundPattern = BackgroundPatternType::NonPeriodicGradient;
	EffectPlatformDX11 platform;
	platform.Initialize(initializingParameter);
	CaptureOrthographicComparisons(platform, manifest, sourceRoot, rootPath, OrthographicView::FrontXY);
	CaptureOrthographicComparisons(platform, manifest, sourceRoot, rootPath, OrthographicView::XZ);
	platform.Terminate();
}

TestRegister Runtime_RenderingTransformScreenshot_DX11(
	"Runtime.RenderingTransformScreenshot.DX11",
	[]() -> void
	{ RunRenderingTransformScreenshotTest(); });

#endif

} // namespace
