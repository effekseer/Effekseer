#pragma once

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <functional>
#include <map>
#include <memory>
#include <numeric>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>

std::string GetDirectoryPath(const char* path);

std::u16string GetDirectoryPathAsU16(const char* path);

std::vector<uint8_t> LoadFile(const char16_t* path);

struct InternalTestHelper;

struct ParsedArgs
{
	std::string Filter;
};

class TestHelper
{
private:
	static std::shared_ptr<InternalTestHelper> Get();

public:
	static ParsedArgs ParseArg(int argc, char* argv[]);

	static void Run(const ParsedArgs& args);

	static void RegisterTest(const char* name, std::function<void()> func);
};

struct TestRegister
{
	TestRegister(const char* name, std::function<void()> func)
	{
		TestHelper::RegisterTest(name, func);
	}
};

#define EXPECT_TRUE(condition)                                          \
	if (!(condition))                                                   \
	{                                                                   \
		printf("%s(%d): FAILED: " #condition "\n", __FILE__, __LINE__); \
		abort();                                                        \
	}

struct Performance
{
	uint32_t min, max, average, median;

	void Print(const char* label)
	{
		printf("%s: Min=%u, Max=%u, Ave=%u, Med:%u\n", label, min, max, average, median);
	}
};

template <class Func>
inline Performance TestPerformance(size_t iterationCount, Func&& func)
{
	using namespace std::chrono;

	std::vector<int64_t> counts;

	for (size_t i = 0; i < iterationCount; i++)
	{
		auto t0 = high_resolution_clock::now();

		func();

		auto t1 = high_resolution_clock::now();

		counts.push_back(duration_cast<microseconds>(t1 - t0).count());
	}

	std::sort(counts.begin(), counts.end());

	Performance result;
	result.min = static_cast<uint32_t>(counts.front());
	result.max = static_cast<uint32_t>(counts.back());
	result.average = static_cast<uint32_t>(std::accumulate(counts.begin(), counts.end(), 0) / counts.size());
	result.median = counts[counts.size() / 2];
	return result;
};
