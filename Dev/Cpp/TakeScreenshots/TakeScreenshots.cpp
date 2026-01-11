
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../3rdParty/stb/stb_image_write.h"

#ifdef _WIN32

#include <Runtime/EffectPlatformDX11.h>
#endif

#include <Runtime/EffectPlatformGL.h>

#include "../Effekseer/Effekseer/Effekseer.Base.h"
#include "../Effekseer/Effekseer/Noise/Effekseer.CurlNoise.h"

#include "Helper.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace
{
std::vector<std::string> CollectEfkefcFiles(const std::string& directory)
{
	std::vector<std::string> paths;
	std::filesystem::path dir_path(directory);
	if (!std::filesystem::exists(dir_path))
	{
		return paths;
	}

	for (const auto& entry : std::filesystem::directory_iterator(dir_path))
	{
		if (!entry.is_regular_file())
		{
			continue;
		}

		const auto& path = entry.path();
		if (path.extension() == ".efkefc")
		{
			paths.push_back(path.generic_string());
		}
	}

	std::sort(paths.begin(), paths.end());
	return paths;
}
} // namespace

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
	if (ifs.is_open())
	{
		std::string effectFilepath;
		while (std::getline(ifs, effectFilepath))
		{
			playAndTakeScreenShot(GetDirectoryPath(__FILE__) + "./Scripts/processed/1.7/" + effectFilepath);
		}
	}

	{
		const auto temp_dir = GetDirectoryPath(__FILE__) + "../../../TestData/Effects/Temp/";
		const auto temp_effects = CollectEfkefcFiles(temp_dir);
		for (const auto& effect_path : temp_effects)
		{
			playAndTakeScreenShot(effect_path);
		}
	}

	platform->Terminate();
}
