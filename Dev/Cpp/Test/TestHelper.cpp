#include "TestHelper.h"
#include <Effekseer.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../3rdParty/stb/stb_image_write.h"

#if defined(_WIN32)
static std::wstring ToWide(const char* text)
{
	int Len = ::MultiByteToWideChar(CP_ACP, 0, text, -1, NULL, 0);

	wchar_t* pOut = new wchar_t[Len + 1];
	::MultiByteToWideChar(CP_ACP, 0, text, -1, pOut, Len);
	std::wstring Out(pOut);
	delete[] pOut;

	return Out;
}
#endif

std::string GetDirectoryPath(const char* path)
{
	auto p = std::string(path);
	size_t last = -1;
	for (size_t i = 0; i < p.size(); i++)
	{
		if (p[i] == '/' || p[i] == '\\')
		{
			last = i;
		}
	}

	if (last >= 0)
	{
		p.resize(last);
		p += "/";
		return p;
	}

	return "";
}

std::u16string GetDirectoryPathAsU16(const char* path)
{
	auto p = GetDirectoryPath(path);

#ifdef _WIN32
	auto w = ToWide(p.c_str());
	return std::u16string((const char16_t*)w.c_str());
#else
	char16_t dst[512];
	Effekseer::ConvertUtf8ToUtf16(dst, 512, p.c_str());
	return std::u16string(dst);
#endif
}

std::vector<uint8_t> LoadFile(const char16_t* path)
{
	char path8[256];
	Effekseer::ConvertUtf16ToUtf8(path8, 256, path);

	FILE* filePtr = NULL;
#ifdef _WIN32
	_wfopen_s(&filePtr, (const wchar_t*)path, L"rb");
#else
	filePtr = fopen((const char*)path8, "rb");
#endif

	if (filePtr == nullptr)
	{
		printf("Failed to load %s./n", path8);
		assert(0);
	}

	fseek(filePtr, 0, SEEK_END);
	auto size = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);

	std::vector<uint8_t> data;
	data.resize(size);
	fread(data.data(), 1, size, filePtr);
	fclose(filePtr);

	return data;
}

struct InternalTestHelper
{
	std::string Root;
	bool IsCaptureRequired = false;
	std::map<std::string, std::function<void()>> tests;
};

std::shared_ptr<InternalTestHelper> TestHelper::Get()
{
	// HACK for initializing order in some platforms
	static std::shared_ptr<InternalTestHelper> internalTestHelper;

	if (internalTestHelper == nullptr)
	{
		internalTestHelper = std::shared_ptr<InternalTestHelper>(new InternalTestHelper());
	}

	return internalTestHelper;
}

ParsedArgs TestHelper::ParseArg(int argc, char* argv[])
{
	ParsedArgs args;

	bool isVulkanMode = false;
	std::string filter;

	for (int i = 0; i < argc; i++)
	{
		auto v = std::string(argv[i]);

		if (v.find("--filter=") == 0)
		{
			args.Filter = v.substr(strlen("--filter="));
		}
	}

	return args;
}

void TestHelper::Run(const ParsedArgs& args)
{
	auto helper = Get();

	if (args.Filter == "")
	{
		for (auto& f : helper->tests)
		{
			std::cout << "Start : " << f.first << std::endl;
			f.second();
		}
	}
	else
	{
		std::basic_regex<char> re(args.Filter);

		for (auto& f : Get()->tests)
		{
			if (!std::regex_match(f.first, re))
				continue;

			std::cout << "Start : " << f.first << std::endl;
			f.second();
		}
	}
}

void TestHelper::RegisterTest(const char* name, std::function<void()> func)
{
	auto helper = Get();
	helper->tests[name] = func;
}
