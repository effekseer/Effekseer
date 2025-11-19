#include "TestHelper.h"
#include <Effekseer.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include <array>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../3rdParty/stb/stb_image_write.h"

#if defined(_WIN32)
static std::wstring ToWide(std::string_view text)
{
	const auto length = ::MultiByteToWideChar(CP_ACP, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
	std::wstring output(length, L'\0');
	::MultiByteToWideChar(CP_ACP, 0, text.data(), static_cast<int>(text.size()), output.data(), length);
	return output;
}
#endif

std::u16string GetDirectoryPathAsU16(const char* path)
{
	auto p = Effekseer::PathHelper::GetDirectoryName(std::string(path));

#ifdef _WIN32
	auto w = ToWide(p);
	return std::u16string(reinterpret_cast<const char16_t*>(w.c_str()));
#else
	std::array<char16_t, 512> dst{};
	Effekseer::ConvertUtf8ToUtf16(dst.data(), static_cast<int>(dst.size()), p.c_str());
	return std::u16string(dst.data());
#endif
}

std::vector<uint8_t> LoadFile(const char16_t* path)
{
	std::array<char, 256> path8{};
	Effekseer::ConvertUtf16ToUtf8(path8.data(), static_cast<int>(path8.size()), path);

	const auto fsPath = std::filesystem::path(std::u16string(path));

	std::ifstream file(fsPath, std::ios::binary);
	if (!file)
	{
		printf("Failed to load %s.\n", path8.data());
		assert(false);
		return {};
	}

	std::error_code ec;
	const auto fileSize = std::filesystem::file_size(fsPath, ec);
	if (ec != std::error_code())
	{
		printf("Failed to get file size for %s. %s\n", path8.data(), ec.message().c_str());
		assert(false);
		return {};
	}

	std::vector<uint8_t> data(static_cast<size_t>(fileSize));
	if (!file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size())))
	{
		printf("Failed to fully read %s.\n", path8.data());
		assert(false);
		return {};
	}

	return data;
}

struct InternalTestHelper
{
	std::string Root;
	bool IsCaptureRequired = false;
	std::map<std::string, std::function<void()>, std::less<>> Tests;
};

std::shared_ptr<InternalTestHelper> TestHelper::Get()
{
	static auto internalTestHelper = std::make_shared<InternalTestHelper>();
	return internalTestHelper;
}

ParsedArgs TestHelper::ParseArg(int argc, char* argv[])
{
	ParsedArgs args{};

	using namespace std::literals;
	constexpr auto filterPrefix = "--filter="sv;

	for (int i = 0; i < argc; i++)
	{
		const std::string_view arg(argv[i]);

		if (arg.compare(0, filterPrefix.size(), filterPrefix) == 0)
		{
			args.Filter = std::string(arg.substr(filterPrefix.size()));
			try
			{
				args.FilterPattern.emplace(args.Filter);
			}
			catch (const std::regex_error& e)
			{
				std::cerr << "Invalid filter regex '" << args.Filter << "': " << e.what() << std::endl;
			}
		}
	}

	return args;
}

void TestHelper::Run(const ParsedArgs& args)
{
	const auto helper = Get();
	const auto& filter = args.FilterPattern;

	for (const auto& [name, func] : helper->Tests)
	{
		if (filter && !std::regex_match(name, *filter))
		{
			continue;
		}
		std::cout << "Start : " << name << std::endl;
		func();
	}
}

void TestHelper::RegisterTest(const char* name, std::function<void()> func)
{
	auto helper = Get();
	helper->Tests.emplace(name, std::move(func));
}
