#include "../TestHelper.h"
#include "BasicRendering.h"
#include "TestPlatforms.h"
#include <cstdlib>

namespace
{
template <class Platform>
void LifecycleTest(int stage)
{
	Platform platform;
	EffectPlatformInitializingParameter parameters;
	parameters.VSync = false;
	platform.Initialize(parameters);
	EXPECT_TRUE(platform.GetRenderer() != nullptr);
	if (stage >= 2)
	{
		EXPECT_TRUE(platform.Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str()) >= 0);
	}
	if (stage == 1 || stage == 3)
	{
		EXPECT_TRUE(platform.Update());
	}
	platform.Terminate();
}

int32_t CountChangedPixels(const std::vector<uint8_t>& before, const std::vector<uint8_t>& after, int32_t width, int32_t yBegin, int32_t yEnd)
{
	EXPECT_TRUE(before.size() == after.size());

	int32_t changed = 0;
	for (int32_t y = yBegin; y < yEnd; y++)
	{
		for (int32_t x = 0; x < width; x++)
		{
			const auto offset = static_cast<size_t>((y * width + x) * 4);
			const auto delta =
				std::abs(static_cast<int32_t>(before[offset + 0]) - static_cast<int32_t>(after[offset + 0])) +
				std::abs(static_cast<int32_t>(before[offset + 1]) - static_cast<int32_t>(after[offset + 1])) +
				std::abs(static_cast<int32_t>(before[offset + 2]) - static_cast<int32_t>(after[offset + 2]));
			if (delta > 12)
			{
				changed++;
			}
		}
	}

	return changed;
}

template <class Platform>
void TexturelessDistortionVisibilityTest(const std::string& name)
{
	EffectPlatformInitializingParameter param;
	param.BackgroundPattern = BackgroundPatternType::NonPeriodicGradient;

	auto platform = std::make_shared<Platform>();
	platform->Initialize(param);

	EXPECT_TRUE(platform->Draw());
	const auto background = platform->CaptureScreenPixels();

	srand(0);
	platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Distortions1.efk").c_str());
	for (size_t i = 0; i < 30; i++)
	{
		EXPECT_TRUE(platform->Update());
	}

	const auto distorted = platform->CaptureScreenPixels();
	EXPECT_TRUE(platform->TakeScreenshot(("TexturelessDistortion_NonPeriodicGradient_" + name + ".png").c_str()));
	platform->Terminate();

	const auto changedPixels = CountChangedPixels(background, distorted, param.WindowSize[0], 0, param.WindowSize[1] / 2);
	printf("TexturelessDistortion_NonPeriodicGradient changed pixels: %d\n", changedPixels);
	EXPECT_TRUE(changedPixels > 250);
}

struct RegisterRenderingSmokeTests
{
	RegisterRenderingSmokeTests()
	{
		ForEachTestPlatform([](auto type, const char* backend)
							{
			using Platform = typename decltype(type)::Type;
			const std::string name = backend;
			const char* stages[] = {"Initialize", "UpdateNoEffect", "PlaySimple", "UpdateSimple"};
			for (int stage = 0; stage < 4; stage++)
			{
				TestHelper::RegisterTest(("Runtime.Lifecycle." + std::string(stages[stage]) + "." + name).c_str(),
					[stage] { LifecycleTest<Platform>(stage); }, TestExecutionMode::FilterOnly);
			}
			auto registerCases = [name](const char* group, std::initializer_list<std::string_view> cases) {
				// Own the case list: registration outlives the initializer_list.
				const std::vector<std::string_view> selected(cases);
				TestHelper::RegisterTest(("Runtime.RenderingSmoke." + std::string(group) + "." + name).c_str(), [name, selected] {
					EffectPlatformInitializingParameter parameters;
					Platform platform;
					BasicRuntimeTestPlatformCases(parameters, &platform, "", "_" + name, selected);
					platform.Terminate();
				}, TestExecutionMode::FilterOnly);
			};
			registerCases("ModelColor", {"Model_Parameters1"});
			registerCases("MaterialUV", {"Material_UV1", "Material_UV2"});
			registerCases("Screenshots", {"SimpleLaser", "Ribbon_Parameters1", "Ring_Parameters1", "Track_Parameters1", "Sprite_Parameters1",
				"Distortions1", "Model_Parameters1", "Lighing_Parameters1", "DynamicParameter1", "BasicRenderSettings_Blend",
				"Material_Sampler1", "Material_UV1", "Material_CustomData1", "Material_CustomDataMax", "SoftParticle01_NotFlipped", "SoftParticle01_Flipped"}); });
		ForEachLLGITestPlatform([](auto type, const char* backend)
								{
			using Platform = typename decltype(type)::Type;
			const std::string name = backend;
			TestHelper::RegisterTest(("Runtime.TexturelessDistortionVisibility." + name).c_str(),
				[name] { TexturelessDistortionVisibilityTest<Platform>(name); }, TestExecutionMode::FilterOnly); });
	}
} registerRenderingSmokeTests;
} // namespace
