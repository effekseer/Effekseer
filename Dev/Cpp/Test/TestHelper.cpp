#include "TestHelper.h"
#include <Effekseer.h>
#ifdef _WIN32
#include <Windows.h>
#endif

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
	Effekseer::ConvertUtf8ToUtf16((int16_t*)dst, 512, (const int8_t*)p.c_str());
	return std::u16string(dst);
#endif
}
