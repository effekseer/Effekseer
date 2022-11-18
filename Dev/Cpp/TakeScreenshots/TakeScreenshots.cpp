
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../3rdParty/stb/stb_image_write.h"

#ifdef _WIN32

#ifdef __EFFEKSEER_BUILD_DX12__
#include <Runtime/EffectPlatformDX12.h>
#endif

#include <Runtime/EffectPlatformDX11.h>
#include <Runtime/EffectPlatformDX9.h>
#elif defined(__APPLE__)
#include <Runtime/EffectPlatformMetal.h>
#else
#endif

#include <Runtime/EffectPlatformGL.h>

#ifdef __EFFEKSEER_BUILD_VULKAN__
#include <Runtime/EffectPlatformVulkan.h>
#endif

#include "../Effekseer/Effekseer/Effekseer.Base.h"
#include "../Effekseer/Effekseer/Noise/CurlNoise.h"

#include "Helper.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

void TakeScreenshots()
{
#ifdef _WIN32
	auto platform = std::make_shared<EffectPlatformDX11>();
#else
	auto platform = std::make_shared<EffectPlatformGL>();
#endif

	EffectPlatformInitializingParameter param;

	platform->Initialize(param);

	auto playAndTakeScreenShot = [&platform](const std::string& path)
	{
		srand(0);
		auto u16Str = ToU16Str(path.c_str());
		platform->Play(u16Str.c_str());

		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}

		std::string screenshotPath = GetParentDirectory(path) + "_" + GetFileNameWithoutExtention(path) + ".png";
		platform->TakeScreenshot(screenshotPath.c_str());

		platform->StopAllEffects();
		for (size_t i = 0; i < 5; i++)
		{
			platform->Update();
		}
	};

	// take screenshots of filepath in efkefclist.txt
	std::ifstream ifs(GetDirectoryPath(__FILE__) + "./Scripts/processed/1.7/efkefclist.txt");
	if (!ifs.is_open())
	{
		return;
	}
	std::string effectFilepath;
	while (std::getline(ifs, effectFilepath))
	{
		playAndTakeScreenShot(GetDirectoryPath(__FILE__) + "./Scripts/processed/1.7/" + effectFilepath);
	}

	platform->Terminate();
}
