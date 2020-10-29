#pragma once

#include <functional>
#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

std::string GetDirectoryPath(const char* path);

std::u16string GetDirectoryPathAsU16(const char* path);

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
