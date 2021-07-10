
#ifdef _WIN32

#ifdef __EFFEKSEER_BUILD_DX12__
#include "EffectPlatformDX12.h"
#endif

#include "EffectPlatformDX11.h"
#include "EffectPlatformDX9.h"
#include "EffectPlatformGL.h"
#elif defined(__APPLE__)
#include "EffectPlatformGL.h"
#include "EffectPlatformMetal.h"
#else
#include "EffectPlatformGL.h"
#endif

#ifdef __EFFEKSEER_BUILD_VULKAN__
#include "EffectPlatformVulkan.h"
#endif

#include "../TestHelper.h"
#include <fstream>
#include <iostream>
#include <sstream>

void CheckPerformancePlatform(EffectPlatform* platform, std::string baseResultPath, std::string suffix)
{
	EffectPlatformInitializingParameter param;
	param.WindowSize = {1280, 720};
	platform->Initialize(param);

	auto test = [&](const char16_t* name, const char* savename) -> void {
		srand(0);

		for (float y = -2.0f; y <= 2.0f; y += 2.0f)
		{
			for (float x = -2.0f; x <= 2.0f; x += 2.0f)
			{
				platform->Play((GetDirectoryPathAsU16(__FILE__) + u"" + name).c_str(), {x, y, 0.0f});
			}
		}

		std::stringstream ss;

		for (size_t i = 0; i < 300; i++)
		{
			platform->Update();

			int32_t updateTime = platform->GetManager()->GetUpdateTime();
			int32_t drawTime = platform->GetManager()->GetDrawTime();

			ss << updateTime << "," << drawTime << std::endl;
		}

		platform->StopAllEffects();

		std::ofstream ofs(savename + suffix + ".csv");
		ofs << ss.str();
	};

	test(u"../../../../TestData/Effects/Performance/Benediction.efkefc", "Benediction");
	test(u"../../../../ResourceData/samples/03_Hanmado01/Effect/hit_hanmado_0409.efkefc", "hit_hanmado_0409");
	test(u"../../../../ResourceData/samples/02_Tktk03/ToonWater.efkefc", "ToonWater");
}

void CheckPerformance()
{
#ifdef _WIN32
	{
		auto platform = std::make_shared<EffectPlatformDX11>();
		CheckPerformancePlatform(platform.get(), "", "_DX11");
		platform->Terminate();
	}
#endif

#if !defined(__FROM_CI__)

#ifdef _WIN32
	{
		auto platform = std::make_shared<EffectPlatformDX9>();
		CheckPerformancePlatform(platform.get(), "", "_DX9");
		platform->Terminate();
	}

#endif

#ifdef __EFFEKSEER_BUILD_DX12__
	{
		auto platform = std::make_shared<EffectPlatformDX12>();
		CheckPerformancePlatform(platform.get(), "", "_DX12");
		platform->Terminate();
	}
#endif

#ifdef __EFFEKSEER_BUILD_VULKAN__
	{
		auto platform = std::make_shared<EffectPlatformVulkan>();
		CheckPerformancePlatform(platform.get(), "", "_Vulkan");
		platform->Terminate();
	}
#endif

#if defined(__APPLE__)
	{
		auto platform = std::make_shared<EffectPlatformMetal>();
		CheckPerformancePlatform(platform.get(), "", "_Metal");
		platform->Terminate();
	}
#endif
	{
		auto platform = std::make_shared<EffectPlatformGL>();
		CheckPerformancePlatform(platform.get(), "", "_GL");
		platform->Terminate();
	}
#endif
}

#if defined(__linux__) || defined(__APPLE__) || defined(WIN32)

TestRegister Performance_CheckPerformance("Performance.CheckPerformance", []() -> void { CheckPerformance(); });

#endif