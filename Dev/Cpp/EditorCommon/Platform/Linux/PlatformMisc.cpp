#include "../PlatformMisc.h"

#include <locale.h>
#include <string>

namespace Effekseer
{

SystemLanguage GetSystemLanguage()
{
	auto localeStr = std::string(setlocale(LC_ALL, NULL));

	if (localeStr.find("ja-") != std::string::npos)
		return SystemLanguage::Japanese;

	if (localeStr.find("en") != std::string::npos)
		return SystemLanguage::English;

	if (localeStr.find("zh_TW") != std::string::npos || localeStr.find("zh-TW") != std::string::npos ||
		localeStr.find("zh_HK") != std::string::npos || localeStr.find("zh-HK") != std::string::npos)
		return SystemLanguage::TraditionalChinese;

	if (localeStr.find("zh-") != std::string::npos || localeStr.find("zh_") != std::string::npos)
		return SystemLanguage::SimplifiedChinese;

	return SystemLanguage::Unknown;
}

} // namespace Effekseer
