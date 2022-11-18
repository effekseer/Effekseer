#include "Helper.h"

#if _WIN32
#include <windows.h>
#endif

#include <Effekseer.h>

#if defined(_WIN32)
std::wstring ToWide(const char* text)
{
	int Len = ::MultiByteToWideChar(CP_ACP, 0, text, -1, NULL, 0);

	wchar_t* pOut = new wchar_t[Len + 1];
	::MultiByteToWideChar(CP_ACP, 0, text, -1, pOut, Len);
	std::wstring Out(pOut);
	delete[] pOut;

	return Out;
}
#endif

std::u16string ToU16Str(const char* text)
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

std::string GetFileNameWithoutExtention(const std::string& path)
{
	std::string::size_type posBefore;
	std::string::size_type posAfter;
	posAfter = path.find_last_of(".");
	posBefore = path.find_last_of("\\/");
	if (posBefore == std::string::npos || posAfter == std::string::npos)
	{
		return path;
	}
	return path.substr(posBefore + 1, posAfter - posBefore - 1);
}

std::string GetParentDirectory(const std::string& path)
{
	std::string::size_type posBefore;
	std::string::size_type posAfter;
	posAfter = path.find_last_of("\\/");
	posBefore = path.find_last_of("\\/", posAfter - 1);

	if (posBefore == std::string::npos || posAfter == std::string::npos)
	{
		return "";
	}
	return path.substr(posBefore + 1, posAfter - posBefore - 1);
}

