#ifdef _WIN32

#ifdef __EFFEKSEER_BUILD_DX12__
#include "EffectPlatformDX12.h"
#endif

#include "EffectPlatformDX11.h"
#include "EffectPlatformDX9.h"
#elif defined(__APPLE__)
#include "EffectPlatformMetal.h"
#else
#endif

#include "EffectPlatformGL.h"

#ifdef __EFFEKSEER_BUILD_VULKAN__
#include "EffectPlatformVulkan.h"
#endif

#include "../Effekseer/Effekseer/Effekseer.Base.h"
#include "../Effekseer/Effekseer/Noise/CurlNoise.h"
#include "../TestHelper.h"
#include <iostream>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

static std::u16string ToU16Str(const char* text)
{
#ifdef _WIN32
	int Len = ::MultiByteToWideChar(CP_ACP, 0, text, -1, NULL, 0);

	wchar_t* pOut = new wchar_t[Len + 1];
	::MultiByteToWideChar(CP_ACP, 0, text, -1, pOut, Len);
	std::u16string Out((const char16_t*)pOut);
	delete[] pOut;
	return Out;
	
#else
	char16_t dst[512];
	Effekseer::ConvertUtf8ToUtf16(dst, 512, text);
	return std::u16string(dst);
#endif

}

void TakeScreenshotsTest()
{
	{
		srand(0);
#ifdef _WIN32
		auto platform = std::make_shared<EffectPlatformDX11>();
#else
		auto platform = std::make_shared<EffectPlatformGL>();
#endif

		EffectPlatformInitializingParameter param;

		platform->Initialize(param);
		
		auto playAndTakeScreenShot = [&platform](std::u16string_view path) 
		{
			platform->Play(path.data());

			for (size_t i = 0; i < 20; i++)
			{
				platform->Update();
			}

			std::filesystem::path effectPath = path;

			std::string screenshotPath = effectPath.parent_path().stem().string() + "_" + effectPath.stem().string() + ".png";
			platform->TakeScreenshot(screenshotPath.c_str());

			platform->StopAllEffects();
			for (size_t i = 0; i < 5; i++)
			{
				platform->Update();
			}
		};


		std::ifstream ifs(GetDirectoryPath(__FILE__) + "../Scripts/processed/1.7/efkefclist.txt");
		if (!ifs.is_open())
		{
			return;
		}
		std::string effectFilepath;
		while (std::getline(ifs, effectFilepath))
		{
			auto u16Str = ToU16Str(effectFilepath.c_str());
			playAndTakeScreenShot(GetDirectoryPathAsU16(__FILE__) + u"../Scripts/processed/1.7/" + u16Str);		
		}
		

		platform->Terminate();
	}
}


#if defined(__linux__) || defined(__APPLE__) || defined(WIN32)

TestRegister Runtime_TakeScreenshotsTest("Runtime.TakeScreenshotsTest", []() -> void
											 { TakeScreenshotsTest(); });

#endif