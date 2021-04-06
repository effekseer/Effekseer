#include "../PlatformMisc.h"

#include <Windows.h>
#include <string>

namespace Effekseer
{

SystemLanguage GetSystemLanguage()
{
	auto localeID = GetUserDefaultUILanguage();
	auto primaryLangID = PRIMARYLANGID(localeID);

	if (primaryLangID == LANG_JAPANESE)
	{
		return SystemLanguage::Japanese;
	}
	else if (primaryLangID == LANG_ENGLISH)
	{
		return SystemLanguage::English;
	}

	return SystemLanguage::Unknown;
}

std::string GetLanguageKey(SystemLanguage language)
{
	if (language == SystemLanguage::Japanese)
	{
		return "ja";
	}
	else if (language == SystemLanguage::English)
	{
		return "en";
	}

	// Fallback
	return "en";
}

SystemLanguage GetSystemLanguage(const std::string& key)
{
	if (key == "ja")
	{
		return SystemLanguage::Japanese;
	}
	else if (key == "en")
	{
		return SystemLanguage::English;
	}

	return SystemLanguage::Unknown;
}

} // namespace Effekseer
