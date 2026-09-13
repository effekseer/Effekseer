#include "../Effekseer/Effekseer/Effekseer.Base.h"
#include "../TestHelper.h"
#include "TestPlatforms.h"
#include <iostream>

void TextureFormatsPlatform(EffectPlatform* platform, std::string baseResultPath, std::string suffix)
{
	EffectPlatformInitializingParameter param;
	platform->Initialize(param);

	auto single15nowebglTest = [&](const char16_t* name, const char* savename) -> void
	{
		srand(0);
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/15_NoWebGL/" + name + u".efkefc").c_str());

		for (size_t i = 0; i < 30; i++)
		{
			EXPECT_TRUE(platform->Update());
		}
		EXPECT_TRUE(platform->TakeScreenshot((std::string(baseResultPath) + savename + suffix + ".png").c_str()));
		platform->StopAllEffects();
	};

	auto single16Test = [&](const char16_t* name, const char* savename) -> void
	{
		srand(0);
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/16/" + name + u".efkefc").c_str());

		for (size_t i = 0; i < 30; i++)
		{
			EXPECT_TRUE(platform->Update());
		}
		EXPECT_TRUE(platform->TakeScreenshot((std::string(baseResultPath) + savename + suffix + ".png").c_str()));
		platform->StopAllEffects();
	};

	single15nowebglTest(u"dds", "dds");
	single16Test(u"TGA01", "TGA01");
}

namespace
{
struct RegisterTextureFormatTests
{
	RegisterTextureFormatTests()
	{
		ForEachTestPlatform([](auto type, const char* backend)
							{
			using Platform = typename decltype(type)::Type;
			const std::string name = backend;
			TestHelper::RegisterTest(("Runtime.TextureFormats." + name).c_str(), [name] {
				Platform platform;
				TextureFormatsPlatform(&platform, "", "_" + name);
				platform.Terminate();
			}, TestExecutionMode::FilterOnly); });
	}
} registerTextureFormatTests;
}
