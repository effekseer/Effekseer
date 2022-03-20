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

	return SystemLanguage::Unknown;
}

} // namespace Effekseer
