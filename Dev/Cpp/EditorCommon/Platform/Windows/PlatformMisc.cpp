#include "../PlatformMisc.h"

#include <string>
#include <windows.h>

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
	else if (primaryLangID == LANG_CHINESE)
	{
		auto subLangID = SUBLANGID(localeID);
		if (subLangID == SUBLANG_CHINESE_TRADITIONAL || subLangID == SUBLANG_CHINESE_HONGKONG || subLangID == SUBLANG_CHINESE_MACAU)
		{
			return SystemLanguage::TraditionalChinese;
		}
		return SystemLanguage::SimplifiedChinese;
	}

	return SystemLanguage::Unknown;
}

} // namespace Effekseer
