
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

struct PerformanceResult
{
	std::vector<int> updateTime;
	std::vector<int> drawTime;
};

const int PerformanceLength = 300;

std::map<std::string, PerformanceResult> CheckPerformancePlatform(EffectPlatform* platform)
{
	EffectPlatformInitializingParameter param;
	param.WindowSize = {1280, 720};
	platform->Initialize(param);

	auto test = [&](const char16_t* name) -> PerformanceResult {
		srand(0);

		PerformanceResult ret;

		const float eps = 0.0001f;

		for (float y = -2.0f; y <= 2.0f + eps; y += 2.0f)
		{
			for (float x = -2.0f; x <= 2.0f + eps; x += 2.0f)
			{
				platform->Play((GetDirectoryPathAsU16(__FILE__) + u"" + name).c_str(), {x, y, 0.0f});
			}
		}

		for (size_t i = 0; i < PerformanceLength; i++)
		{
			platform->Update();

			int32_t updateTime = platform->GetManager()->GetUpdateTime();
			int32_t drawTime = platform->GetManager()->GetDrawTime();
			ret.updateTime.emplace_back(updateTime);
			ret.drawTime.emplace_back(drawTime);
		}

		platform->StopAllEffects();

		return ret;
	};

	std::map<std::string, PerformanceResult> ret;
	ret["ManyRings"] = test(u"../../../../TestData/Effects/Performance/ManyRings.efkefc");
	ret["Benediction"] = test(u"../../../../TestData/Effects/Performance/Benediction.efkefc");
	ret["hit_hanmado_0409"] = test(u"../../../../ResourceData/samples/03_Hanmado01/Effect/hit_hanmado_0409.efkefc");
	ret["ToonWater"] = test(u"../../../../ResourceData/samples/02_Tktk03/ToonWater.efkefc");
	return ret;
}

void CheckPerformance()
{
	std::map<std::string, std::map<std::string, PerformanceResult>> results;

	{
		auto platform = std::make_shared<EffectPlatform>();
		const auto result = CheckPerformancePlatform(platform.get());
		platform->Terminate();

		for (const auto& r : result)
		{
			results[r.first]["None"] = r.second;
		}
	}
#ifdef _WIN32
	{
		auto platform = std::make_shared<EffectPlatformDX11>();
		const auto result = CheckPerformancePlatform(platform.get());
		platform->Terminate();

		for (const auto& r : result)
		{
			results[r.first]["DX11"] = r.second;
		}
	}
#endif

#if !defined(__FROM_CI__)

#ifdef _WIN32
	{
		auto platform = std::make_shared<EffectPlatformDX9>();
		const auto result = CheckPerformancePlatform(platform.get());
		platform->Terminate();

		for (const auto& r : result)
		{
			results[r.first]["DX9"] = r.second;
		}
	}

#endif

#ifdef __EFFEKSEER_BUILD_DX12__
	{
		auto platform = std::make_shared<EffectPlatformDX12>();
		const auto result = CheckPerformancePlatform(platform.get());
		platform->Terminate();

		for (const auto& r : result)
		{
			results[r.first]["DX12"] = r.second;
		}
	}
#endif

#ifdef __EFFEKSEER_BUILD_VULKAN__
	{
		auto platform = std::make_shared<EffectPlatformVulkan>();
		const auto result = CheckPerformancePlatform(platform.get());
		platform->Terminate();

		for (const auto& r : result)
		{
			results[r.first]["Vulkan"] = r.second;
		}
	}
#endif

#if defined(__APPLE__)
	{
		auto platform = std::make_shared<EffectPlatformMetal>();
		const auto result = CheckPerformancePlatform(platform.get());
		platform->Terminate();

		for (const auto& r : result)
		{
			results[r.first]["Metal"] = r.second;
		}
	}
#endif
	{
		auto platform = std::make_shared<EffectPlatformGL>();
		const auto result = CheckPerformancePlatform(platform.get());
		platform->Terminate();

		for (const auto& r : result)
		{
			results[r.first]["GL"] = r.second;
		}
	}
#endif

	for (const auto& r : results)
	{
		std::stringstream ss;

		for (const auto& p : r.second)
		{
			ss << "Update(" << p.first << "),";
		}

		for (const auto& p : r.second)
		{
			ss << "Draw(" << p.first << "),";
		}

		ss << std::endl;

		for (int i = 0; i < PerformanceLength; i++)
		{
			for (const auto& p : r.second)
			{
				ss << p.second.updateTime[i] << ",";
			}

			for (const auto& p : r.second)
			{
				ss << p.second.drawTime[i] << ",";
			}

			ss << std::endl;
		}

		std::ofstream ofs(r.first + ".csv");
		ofs << ss.str();
	}
}

#if defined(__linux__) || defined(__APPLE__) || defined(WIN32)

TestRegister Performance_CheckPerformance("Performance.CheckPerformance", []() -> void { CheckPerformance(); });

#endif