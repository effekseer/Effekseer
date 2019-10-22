#include "../PlatformMisc.h"

#include <Windows.h>
#include <string>

namespace Effekseer
{

SystemLanguage GetSystemLanguage()
{
	wchar_t locale[LOCALE_NAME_MAX_LENGTH];

	if (GetSystemDefaultLocaleName(locale, LOCALE_NAME_MAX_LENGTH))
	{
		auto localeStr = std::wstring(locale);

		if (localeStr.find(L"ja-") != std::string::npos)
			return SystemLanguage::Japanese;

		if (localeStr.find(L"en") != std::string::npos)
			return SystemLanguage::English;
	}

	return SystemLanguage::Unknown;
}

} // namespace Effekseer
