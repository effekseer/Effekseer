#ifdef _WIN32
#include <Runtime/EffectPlatformDX11.h>
#endif

#include <Runtime/EffectPlatform.h>

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
};

struct RenderingTransformScreenshotVariant
{
	const char* Label;
	Effekseer::EffectFlipParameter Flip;
	bool ReflectY;
};

const std::array<RenderingTransformScreenshotCase, 6> ScreenshotCases = {{
	{"Sprite", u"TestData/Effects/Update_17x/Sprite.efkefc", 30},
	{"Ribbon", u"TestData/Effects/10/Ribbon_Parameters1.efk", 30},
	{"Ring", u"TestData/Effects/10/Ring_Parameters1.efk", 30},
	{"Track", u"TestData/Effects/Update_17x/Track.efkefc", 30},
	{"Model", u"TestData/Effects/Update_17x/Model.efkefc", 30},
	{"GpuParticles", u"TestData/Effects/18/GpuParticles_sprite_simple.efkefc", 120},
}};

const std::array<RenderingTransformScreenshotVariant, 6> ScreenshotVariants = {{
	{"UNCHANGED", {}, false},
	{"FLIP-X", {true, false, false}, false},
	{"FLIP-Y", {false, true, false}, false},
	{"FLIP-Z", {false, false, true}, false},
	{"REFLECT-Y", {}, true},
	{"FLIP-Y+REFLECT-Y", {false, true, false}, true},
}};

Effekseer::Matrix44 MakeRenderingCoordinateMatrix(bool reflectY)
{
	Effekseer::Matrix44 matrix;
	if (reflectY)
	{
		matrix.Scaling(1.0f, -1.0f, 1.0f);
	}
	return matrix;
}

void RunRenderingTransformScreenshotTest()
{
	const std::filesystem::path sourceRoot = "screenshot_comparison_sources";
	const auto manifestPath = sourceRoot / "manifest.csv";
	std::filesystem::create_directories(sourceRoot);

	std::ofstream manifest(manifestPath, std::ios::trunc);
	EXPECT_TRUE(manifest.is_open());
	manifest << "composite,order,label,source\n";

	EffectPlatformInitializingParameter initializingParameter;
	EffectPlatformDX11 platform;
	platform.Initialize(initializingParameter);

	const auto rootPath = GetDirectoryPathAsU16(__FILE__) + u"../../../../";
	for (const auto& screenshotCase : ScreenshotCases)
	{
		printf("[RenderingTransformScreenshot] case=%s frames=%d\n", screenshotCase.Name, screenshotCase.FrameCount);
		const auto caseDirectory = sourceRoot / "DX11" / screenshotCase.Name;
		std::filesystem::create_directories(caseDirectory);

		srand(0);
		const auto handle = platform.Play(
			(rootPath + screenshotCase.EffectPath).c_str(),
			{2.0f, 1.0f, 0.0f});
		EXPECT_TRUE(handle >= 0);

		for (int32_t frame = 0; frame < screenshotCase.FrameCount; frame++)
		{
			EXPECT_TRUE(platform.Update());
		}

		for (size_t variantIndex = 0; variantIndex < ScreenshotVariants.size(); variantIndex++)
		{
			const auto& variant = ScreenshotVariants[variantIndex];
			printf("[RenderingTransformScreenshot] case=%s variant=%s\n", screenshotCase.Name, variant.Label);
			platform.GetManager()->SetEffectFlip(handle, variant.Flip);
			EXPECT_TRUE(platform.GetManager()->GetEffectFlip(handle) == variant.Flip);
			platform.GetManager()->Flip();
			platform.SetRenderingCoordinateMatrix(MakeRenderingCoordinateMatrix(variant.ReflectY));
			EXPECT_TRUE(platform.Draw());

			const auto sourcePath = caseDirectory /
				(std::to_string(variantIndex) + "_" + variant.Label + ".png");
			EXPECT_TRUE(platform.TakeScreenshot(sourcePath.string().c_str()));

			manifest << "RenderingTransformComparison_" << screenshotCase.Name << "_DX11.png,"
					 << variantIndex << "," << variant.Label << "," << sourcePath.generic_string() << "\n";
		}

		platform.SetRenderingCoordinateMatrix(Effekseer::Matrix44());
		platform.StopAllEffects();
		platform.ClearLoadedEffects();
	}

	platform.Terminate();
}

TestRegister Runtime_RenderingTransformScreenshot_DX11(
	"Runtime.RenderingTransformScreenshot.DX11",
	[]() -> void
	{ RunRenderingTransformScreenshotTest(); });

#endif

} // namespace
