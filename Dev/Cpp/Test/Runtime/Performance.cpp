
#ifdef _WIN32

#ifdef __EFFEKSEER_BUILD_DX12__
#include <Runtime/EffectPlatformDX12.h>
#endif

#include <Runtime/EffectPlatformDX11.h>
#include <Runtime/EffectPlatformDX9.h>
#include <Runtime/EffectPlatformGL.h>
#elif defined(__APPLE__)
#include <Runtime/EffectPlatformGL.h>
#include <Runtime/EffectPlatformMetal.h>
#else
#include <Runtime/EffectPlatformGL.h>
#endif

#ifdef __EFFEKSEER_BUILD_VULKAN__
#include <Runtime/EffectPlatformVulkan.h>
#endif

#include "../TestHelper.h"
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

struct PerformanceResult
{
	std::vector<int> managerUpdateTime;
	std::vector<int> computeTime;
	std::vector<int> platformBeginRenderingTime;
	std::vector<int> rendererBeginTime;
	std::vector<int> managerDrawTime;
	std::vector<int> managerDrawWorkerThreadWaitTime;
	std::vector<int> managerDrawMutexLockTime;
	std::vector<int> managerDrawCullingTime;
	std::vector<int> managerDrawSortingTime;
	std::vector<int> managerDrawDrawSetsTime;
	std::vector<int> managerDrawGpuParticlesTime;
	std::vector<int> managerDrawTotalTime;
	std::vector<int> rendererEndTime;
	std::vector<int> platformEndRenderingTime;
	std::vector<int> frameWithoutPresentTime;
	std::vector<int> presentTime;
	std::vector<int> frameTime;
};

namespace
{

constexpr int32_t DefaultPerformanceFrameCount = 300;
constexpr int32_t DefaultPerformanceWarmupFrameCount = 16;
constexpr int32_t DefaultPerformanceRepeatCount = 1;
constexpr int32_t DefaultPerformanceActiveThreshold = 10;

struct PerformanceOptions
{
	int32_t FrameCount = DefaultPerformanceFrameCount;
	int32_t WarmupFrameCount = DefaultPerformanceWarmupFrameCount;
	int32_t RepeatCount = DefaultPerformanceRepeatCount;
	int32_t ActiveThreshold = DefaultPerformanceActiveThreshold;
	bool IsolateEffects = false;
	bool VSync = true;
	int32_t InstanceCount = 8000;
	int32_t SpriteCount = 2000;
	std::array<int32_t, 2> WindowSize = {1280, 720};
	std::string PlatformFilter;
	std::string EffectFilter;
};

struct PerformanceEffectCase
{
	const char* Name;
	const char16_t* Path;
};

struct PerformanceResultEntry
{
	std::string Platform;
	int32_t Run = 0;
	PerformanceResult Result;
};

using PerformanceResults = std::map<std::string, std::vector<PerformanceResultEntry>>;
using PlatformFactory = std::function<std::shared_ptr<EffectPlatform>()>;

const std::array<PerformanceEffectCase, 7> PerformanceEffects = {{
	{"ManyRings1", u"../../../../TestData/Effects/Performance/ManyRings1.efkefc"},
	{"ManyRings2", u"../../../../TestData/Effects/Performance/ManyRings2.efkefc"},
	{"ManySprites1", u"../../../../TestData/Effects/Performance/ManySprites1.efkefc"},
	{"ManySpritesAdvance1", u"../../../../TestData/Effects/Performance/ManySpritesAdvance1.efkefc"},
	{"Benediction", u"../../../../TestData/Effects/Performance/Benediction.efkefc"},
	{"hit_hanmado_0409", u"../../../../ResourceData/samples/03_Hanmado01/Effect/hit_hanmado_0409.efkefc"},
	{"ToonWater", u"../../../../ResourceData/samples/02_Tktk03/ToonWater.efkefc"},
}};

struct PerformanceMetric
{
	const char* Name;
	std::vector<int> PerformanceResult::* Values;
};

const std::array<PerformanceMetric, 17> PerformanceMetrics = {{
	{"ManagerUpdate", &PerformanceResult::managerUpdateTime},
	{"Compute", &PerformanceResult::computeTime},
	{"PlatformBeginRendering", &PerformanceResult::platformBeginRenderingTime},
	{"RendererBegin", &PerformanceResult::rendererBeginTime},
	{"ManagerDraw", &PerformanceResult::managerDrawTime},
	{"ManagerDrawWorkerThreadWait", &PerformanceResult::managerDrawWorkerThreadWaitTime},
	{"ManagerDrawMutexLock", &PerformanceResult::managerDrawMutexLockTime},
	{"ManagerDrawCulling", &PerformanceResult::managerDrawCullingTime},
	{"ManagerDrawSorting", &PerformanceResult::managerDrawSortingTime},
	{"ManagerDrawDrawSets", &PerformanceResult::managerDrawDrawSetsTime},
	{"ManagerDrawGpuParticles", &PerformanceResult::managerDrawGpuParticlesTime},
	{"ManagerDrawTotal", &PerformanceResult::managerDrawTotalTime},
	{"RendererEnd", &PerformanceResult::rendererEndTime},
	{"PlatformEndRendering", &PerformanceResult::platformEndRenderingTime},
	{"FrameWithoutPresent", &PerformanceResult::frameWithoutPresentTime},
	{"Present", &PerformanceResult::presentTime},
	{"Frame", &PerformanceResult::frameTime},
}};

std::string ToLower(std::string text)
{
	for (auto& c : text)
	{
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	}
	return text;
}

std::string Trim(std::string text)
{
	const auto begin = std::find_if(text.begin(), text.end(), [](unsigned char c) { return !std::isspace(c); });
	const auto end = std::find_if(text.rbegin(), text.rend(), [](unsigned char c) { return !std::isspace(c); }).base();
	if (begin >= end)
	{
		return {};
	}
	return std::string(begin, end);
}

bool EqualsIgnoreCase(const std::string& lhs, const std::string& rhs)
{
	return ToLower(lhs) == ToLower(rhs);
}

std::optional<std::string> FindCommandLineValue(std::initializer_list<const char*> names)
{
	const auto& args = TestHelper::GetCommandLineArgs();
	for (size_t i = 1; i < args.size(); i++)
	{
		for (const auto name : names)
		{
			const std::string optionName = name;
			const auto prefix = optionName + "=";
			const auto& arg = args[i];

			if (arg == optionName && i + 1 < args.size() && args[i + 1].compare(0, 2, "--") != 0)
			{
				return args[i + 1];
			}

			if (arg.compare(0, prefix.size(), prefix) == 0)
			{
				return arg.substr(prefix.size());
			}
		}
	}

	return std::nullopt;
}

std::optional<bool> FindCommandLineBool(std::initializer_list<const char*> names)
{
	const auto parseBool = [](std::string value) -> std::optional<bool>
	{
		value = ToLower(Trim(value));
		if (value == "1" || value == "true" || value == "on" || value == "yes")
		{
			return true;
		}
		if (value == "0" || value == "false" || value == "off" || value == "no")
		{
			return false;
		}
		return std::nullopt;
	};

	const auto& args = TestHelper::GetCommandLineArgs();
	for (size_t i = 1; i < args.size(); i++)
	{
		for (const auto name : names)
		{
			const std::string optionName = name;
			const auto prefix = optionName + "=";
			const auto& arg = args[i];

			if (arg == optionName)
			{
				if (i + 1 < args.size() && args[i + 1].compare(0, 2, "--") != 0)
				{
					if (const auto parsed = parseBool(args[i + 1]))
					{
						return parsed;
					}
				}
				return true;
			}

			if (arg.compare(0, prefix.size(), prefix) == 0)
			{
				return parseBool(arg.substr(prefix.size()));
			}
		}
	}

	return std::nullopt;
}

std::optional<int32_t> ParseInt(const std::string& text)
{
	char* end = nullptr;
	const auto value = std::strtol(text.c_str(), &end, 10);
	if (end == text.c_str() || *end != '\0')
	{
		return std::nullopt;
	}
	return static_cast<int32_t>(value);
}

std::optional<bool> ParseEnvBool(const char* name)
{
	auto value = std::getenv(name);
	if (value == nullptr || value[0] == '\0')
	{
		return std::nullopt;
	}

	auto lower = ToLower(Trim(value));
	if (lower == "1" || lower == "true" || lower == "on" || lower == "yes")
	{
		return true;
	}
	if (lower == "0" || lower == "false" || lower == "off" || lower == "no")
	{
		return false;
	}
	return std::nullopt;
}

int32_t GetOptionInt(std::initializer_list<const char*> names, const char* envName, int32_t defaultValue, int32_t minValue)
{
	if (const auto value = FindCommandLineValue(names))
	{
		if (const auto parsed = ParseInt(*value))
		{
			return (std::max)(*parsed, minValue);
		}
	}

	if (envName != nullptr)
	{
		auto value = std::getenv(envName);
		if (value != nullptr && value[0] != '\0')
		{
			if (const auto parsed = ParseInt(value))
			{
				return (std::max)(*parsed, minValue);
			}
		}
	}

	return defaultValue;
}

std::string GetOptionString(std::initializer_list<const char*> names, const char* envName)
{
	if (const auto value = FindCommandLineValue(names))
	{
		return *value;
	}

	if (envName != nullptr)
	{
		auto value = std::getenv(envName);
		if (value != nullptr)
		{
			return value;
		}
	}

	return {};
}

bool GetOptionBool(std::initializer_list<const char*> names, const char* envName, bool defaultValue)
{
	if (const auto value = FindCommandLineBool(names))
	{
		return *value;
	}

	if (envName != nullptr)
	{
		if (const auto value = ParseEnvBool(envName))
		{
			return *value;
		}
	}

	return defaultValue;
}

PerformanceOptions ReadPerformanceOptions()
{
	EffectPlatformInitializingParameter defaultParam;

	PerformanceOptions options;
	options.FrameCount = GetOptionInt({"--performance-frames", "--performance-frame-count"}, "EFFEKSEER_PERFORMANCE_FRAMES", DefaultPerformanceFrameCount, 1);
	options.WarmupFrameCount =
		GetOptionInt({"--performance-warmup", "--performance-warmup-frames"}, "EFFEKSEER_PERFORMANCE_WARMUP_FRAMES", DefaultPerformanceWarmupFrameCount, 0);
	options.RepeatCount = GetOptionInt({"--performance-repeat", "--performance-repeat-count"}, "EFFEKSEER_PERFORMANCE_REPEAT", DefaultPerformanceRepeatCount, 1);
	options.ActiveThreshold =
		GetOptionInt({"--performance-active-threshold"}, "EFFEKSEER_PERFORMANCE_ACTIVE_THRESHOLD", DefaultPerformanceActiveThreshold, 0);
	options.IsolateEffects = GetOptionBool({"--performance-isolate-effects"}, "EFFEKSEER_PERFORMANCE_ISOLATE_EFFECTS", false);
	options.VSync = GetOptionBool({"--performance-vsync"}, "EFFEKSEER_PERFORMANCE_VSYNC", defaultParam.VSync);
	options.InstanceCount =
		GetOptionInt({"--performance-instance-count"}, "EFFEKSEER_PERFORMANCE_INSTANCE_COUNT", defaultParam.InstanceCount, 1);
	options.SpriteCount = GetOptionInt({"--performance-sprite-count"}, "EFFEKSEER_PERFORMANCE_SPRITE_COUNT", defaultParam.SpriteCount, 1);
	options.WindowSize[0] =
		GetOptionInt({"--performance-window-width"}, "EFFEKSEER_PERFORMANCE_WINDOW_WIDTH", 1280, 1);
	options.WindowSize[1] =
		GetOptionInt({"--performance-window-height"}, "EFFEKSEER_PERFORMANCE_WINDOW_HEIGHT", 720, 1);
	options.PlatformFilter = GetOptionString({"--performance-platform"}, "EFFEKSEER_PERFORMANCE_PLATFORM");
	options.EffectFilter = GetOptionString({"--performance-effect"}, "EFFEKSEER_PERFORMANCE_EFFECT");
	return options;
}

bool MatchesFilter(const char* value, const std::string& filter)
{
	if (filter.empty() || filter == "*")
	{
		return true;
	}

	size_t begin = 0;
	while (begin <= filter.size())
	{
		const auto end = filter.find(',', begin);
		const auto token = Trim(filter.substr(begin, end == std::string::npos ? std::string::npos : end - begin));
		if (!token.empty() && (token == "*" || EqualsIgnoreCase(token, value)))
		{
			return true;
		}

		if (end == std::string::npos)
		{
			break;
		}
		begin = end + 1;
	}

	return false;
}

bool HasSelectedEffect(const PerformanceOptions& options)
{
	return std::any_of(PerformanceEffects.begin(), PerformanceEffects.end(), [&options](const auto& effect) {
		return MatchesFilter(effect.Name, options.EffectFilter);
	});
}

void ReservePerformanceResult(PerformanceResult& result, int32_t frameCount)
{
	for (const auto& metric : PerformanceMetrics)
	{
		(result.*metric.Values).reserve(frameCount);
	}
}

void PushMeasuredTime(PerformanceResult& result, const EffectPlatformMeasuredTime& measuredTime)
{
	result.managerUpdateTime.emplace_back(measuredTime.ManagerUpdate);
	result.computeTime.emplace_back(measuredTime.Compute);
	result.platformBeginRenderingTime.emplace_back(measuredTime.PlatformBeginRendering);
	result.rendererBeginTime.emplace_back(measuredTime.RendererBegin);
	result.managerDrawTime.emplace_back(measuredTime.ManagerDraw);
	result.managerDrawWorkerThreadWaitTime.emplace_back(measuredTime.ManagerDrawWorkerThreadWait);
	result.managerDrawMutexLockTime.emplace_back(measuredTime.ManagerDrawMutexLock);
	result.managerDrawCullingTime.emplace_back(measuredTime.ManagerDrawCulling);
	result.managerDrawSortingTime.emplace_back(measuredTime.ManagerDrawSorting);
	result.managerDrawDrawSetsTime.emplace_back(measuredTime.ManagerDrawDrawSets);
	result.managerDrawGpuParticlesTime.emplace_back(measuredTime.ManagerDrawGpuParticles);
	result.managerDrawTotalTime.emplace_back(measuredTime.ManagerDrawTotal);
	result.rendererEndTime.emplace_back(measuredTime.RendererEnd);
	result.platformEndRenderingTime.emplace_back(measuredTime.PlatformEndRendering);
	result.frameWithoutPresentTime.emplace_back(measuredTime.FrameWithoutPresent);
	result.presentTime.emplace_back(measuredTime.Present);
	result.frameTime.emplace_back(measuredTime.Frame);
}

void PlayPerformanceEffectGrid(EffectPlatform* platform, const char16_t* path)
{
	constexpr float gridMin = -2.0f;
	constexpr float gridMax = 2.0f;
	constexpr float gridStep = 2.0f;
	constexpr float eps = 0.0001f;
	const auto fullPath = GetDirectoryPathAsU16(__FILE__) + u"" + path;

	for (float y = gridMin; y <= gridMax + eps; y += gridStep)
	{
		for (float x = gridMin; x <= gridMax + eps; x += gridStep)
		{
			platform->Play(fullPath.c_str(), {x, y, 0.0f});
		}
	}
}

PerformanceResult MeasurePerformanceEffect(EffectPlatform* platform, const char16_t* path, const PerformanceOptions& options)
{
	srand(0);

	PerformanceResult ret;
	ReservePerformanceResult(ret, options.FrameCount);

	PlayPerformanceEffectGrid(platform, path);

	for (int32_t i = 0; i < options.WarmupFrameCount; i++)
	{
		platform->Update();
	}

	for (int32_t i = 0; i < options.FrameCount; i++)
	{
		EffectPlatformMeasuredTime measuredTime;
		platform->Update(&measuredTime);
		PushMeasuredTime(ret, measuredTime);
	}

	platform->StopAllEffects();
	platform->ClearLoadedEffects();

	return ret;
}

void InitializePerformancePlatform(EffectPlatform* platform, const PerformanceOptions& options)
{
	EffectPlatformInitializingParameter param;
	param.WindowSize = options.WindowSize;
	param.VSync = options.VSync;
	param.InstanceCount = options.InstanceCount;
	param.SpriteCount = options.SpriteCount;
	platform->Initialize(param);
}

void AddPerformanceResult(PerformanceResults& results, const PerformanceEffectCase& effect, const char* platformName, int32_t run, PerformanceResult result)
{
	results[effect.Name].push_back(PerformanceResultEntry{platformName, run, std::move(result)});
}

void MeasurePerformancePlatform(const char* platformName, const PlatformFactory& createPlatform, const PerformanceOptions& options, PerformanceResults& results)
{
	if (!MatchesFilter(platformName, options.PlatformFilter) || !HasSelectedEffect(options))
	{
		return;
	}

	for (int32_t run = 0; run < options.RepeatCount; run++)
	{
		if (options.IsolateEffects)
		{
			for (const auto& effect : PerformanceEffects)
			{
				if (!MatchesFilter(effect.Name, options.EffectFilter))
				{
					continue;
				}

				auto platform = createPlatform();
				InitializePerformancePlatform(platform.get(), options);
				AddPerformanceResult(results, effect, platformName, run, MeasurePerformanceEffect(platform.get(), effect.Path, options));
				platform->Terminate();
			}
		}
		else
		{
			auto platform = createPlatform();
			InitializePerformancePlatform(platform.get(), options);

			for (const auto& effect : PerformanceEffects)
			{
				if (!MatchesFilter(effect.Name, options.EffectFilter))
				{
					continue;
				}

				AddPerformanceResult(results, effect, platformName, run, MeasurePerformanceEffect(platform.get(), effect.Path, options));
			}

			platform->Terminate();
		}
	}
}

std::string GetEntryLabel(const PerformanceResultEntry& entry, const PerformanceOptions& options)
{
	if (options.RepeatCount <= 1)
	{
		return entry.Platform;
	}

	return entry.Platform + ".Run" + std::to_string(entry.Run + 1);
}

void StorePerformanceResults(const PerformanceResults& results, const PerformanceOptions& options)
{
	for (const auto& effectResult : results)
	{
		std::stringstream ss;

		for (const auto& metric : PerformanceMetrics)
		{
			for (const auto& entry : effectResult.second)
			{
				ss << metric.Name << "(" << GetEntryLabel(entry, options) << "),";
			}
		}

		ss << std::endl;

		for (int32_t frameIndex = 0; frameIndex < options.FrameCount; frameIndex++)
		{
			for (const auto& metric : PerformanceMetrics)
			{
				for (const auto& entry : effectResult.second)
				{
					const auto& values = entry.Result.*metric.Values;
					if (frameIndex < static_cast<int32_t>(values.size()))
					{
						ss << values[frameIndex];
					}
					ss << ",";
				}
			}

			ss << std::endl;
		}

		std::ofstream ofs(effectResult.first + ".csv");
		ofs << ss.str();
	}
}

struct SummaryStats
{
	int32_t Min = 0;
	int32_t Median = 0;
	double Average = 0.0;
	int32_t P90 = 0;
	int32_t P95 = 0;
	int32_t Max = 0;
};

SummaryStats CalculateSummaryStats(std::vector<int> values)
{
	if (values.empty())
	{
		return {};
	}

	std::sort(values.begin(), values.end());

	const auto percentile = [&values](int32_t percent) -> int32_t
	{
		const auto index = static_cast<size_t>((static_cast<int64_t>(values.size() - 1) * percent) / 100);
		return values[index];
	};

	SummaryStats stats;
	stats.Min = values.front();
	stats.Median = percentile(50);
	stats.Average = std::accumulate(values.begin(), values.end(), 0.0) / static_cast<double>(values.size());
	stats.P90 = percentile(90);
	stats.P95 = percentile(95);
	stats.Max = values.back();
	return stats;
}

std::vector<size_t> GetActiveFrameIndices(const PerformanceResult& result, const PerformanceOptions& options)
{
	std::vector<size_t> indices;
	for (size_t i = 0; i < result.managerDrawTime.size(); i++)
	{
		const auto activeTime = (std::max)(result.managerUpdateTime[i], result.managerDrawTime[i]);
		if (options.ActiveThreshold == 0 || activeTime >= options.ActiveThreshold)
		{
			indices.emplace_back(i);
		}
	}
	return indices;
}

std::vector<int> GetActiveValues(const PerformanceResult& result, const PerformanceMetric& metric, const std::vector<size_t>& activeFrameIndices)
{
	const auto& values = result.*metric.Values;
	std::vector<int> activeValues;
	activeValues.reserve(activeFrameIndices.size());

	for (const auto index : activeFrameIndices)
	{
		if (index < values.size())
		{
			activeValues.emplace_back(values[index]);
		}
	}

	return activeValues;
}

void StorePerformanceSummary(const PerformanceResults& results, const PerformanceOptions& options)
{
	std::ofstream ofs("PerformanceSummary.csv");
	ofs << "Effect,Platform,Run,Metric,Frames,ActiveFrames,ActiveThreshold,WarmupFrames,IsolateEffects,VSync,WindowWidth,WindowHeight,InstanceCount,SpriteCount,Min,Median,Average,P90,P95,Max\n";

	for (const auto& effectResult : results)
	{
		for (const auto& entry : effectResult.second)
		{
			const auto activeFrameIndices = GetActiveFrameIndices(entry.Result, options);
			for (const auto& metric : PerformanceMetrics)
			{
				const auto stats = CalculateSummaryStats(GetActiveValues(entry.Result, metric, activeFrameIndices));
				ofs << effectResult.first << ",";
				ofs << entry.Platform << ",";
				ofs << (entry.Run + 1) << ",";
				ofs << metric.Name << ",";
				ofs << options.FrameCount << ",";
				ofs << activeFrameIndices.size() << ",";
				ofs << options.ActiveThreshold << ",";
				ofs << options.WarmupFrameCount << ",";
				ofs << (options.IsolateEffects ? 1 : 0) << ",";
				ofs << (options.VSync ? 1 : 0) << ",";
				ofs << options.WindowSize[0] << ",";
				ofs << options.WindowSize[1] << ",";
				ofs << options.InstanceCount << ",";
				ofs << options.SpriteCount << ",";
				ofs << stats.Min << ",";
				ofs << stats.Median << ",";
				ofs << stats.Average << ",";
				ofs << stats.P90 << ",";
				ofs << stats.P95 << ",";
				ofs << stats.Max << "\n";
			}
		}
	}
}

void PrintPerformanceOptions(const PerformanceOptions& options)
{
	std::cout << "Performance options: "
			  << "platform=" << (options.PlatformFilter.empty() ? "*" : options.PlatformFilter) << ", "
			  << "effect=" << (options.EffectFilter.empty() ? "*" : options.EffectFilter) << ", "
			  << "frames=" << options.FrameCount << ", "
			  << "warmup=" << options.WarmupFrameCount << ", "
			  << "repeat=" << options.RepeatCount << ", "
			  << "activeThreshold=" << options.ActiveThreshold << ", "
			  << "isolateEffects=" << (options.IsolateEffects ? "true" : "false") << ", "
			  << "vsync=" << (options.VSync ? "true" : "false") << std::endl;
}

} // namespace

void CheckPerformance()
{
	const auto options = ReadPerformanceOptions();
	PrintPerformanceOptions(options);

	PerformanceResults results;

	MeasurePerformancePlatform("None", []() { return std::make_shared<EffectPlatform>(); }, options, results);

#ifdef _WIN32
	MeasurePerformancePlatform("DX11", []() { return std::make_shared<EffectPlatformDX11>(); }, options, results);
#endif

#if !defined(__FROM_CI__)

#ifdef _WIN32
	MeasurePerformancePlatform("DX9", []() { return std::make_shared<EffectPlatformDX9>(); }, options, results);
#endif

#ifdef __EFFEKSEER_BUILD_DX12__
	MeasurePerformancePlatform("DX12", []() { return std::make_shared<EffectPlatformDX12>(); }, options, results);
#endif

#ifdef __EFFEKSEER_BUILD_VULKAN__
	MeasurePerformancePlatform("Vulkan", []() { return std::make_shared<EffectPlatformVulkan>(); }, options, results);
#endif

#if defined(__APPLE__)
	MeasurePerformancePlatform("Metal", []() { return std::make_shared<EffectPlatformMetal>(); }, options, results);
#endif

	MeasurePerformancePlatform("GL", []() { return std::make_shared<EffectPlatformGL>(); }, options, results);
#endif

	StorePerformanceResults(results, options);
	StorePerformanceSummary(results, options);
}

#if defined(__linux__) || defined(__APPLE__) || defined(WIN32)

TestRegister Performance_CheckPerformance("Performance.CheckPerformance", []() -> void
										  { CheckPerformance(); });

#endif
